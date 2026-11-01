import os
import re
import uuid
import io
from datetime import datetime

from flask import Flask, request, jsonify, session, send_from_directory, send_file
from flask_cors import CORS
from werkzeug.utils import secure_filename

from database import Database, DB_PATH
from auth import AuthService
from image_processor import ImageProcessor
from quality_detector import BasicQualityDetector
from inspection_manager import InspectionManager
from dashboard import Dashboard
from report import ReportGenerator
from contact import ContactService

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT_ROOT = os.path.dirname(BASE_DIR)
FRONTEND_DIR = os.path.join(PROJECT_ROOT, "frontend")
UPLOADS_DIR = os.path.join(PROJECT_ROOT, "uploads")

app = Flask(__name__, static_folder=FRONTEND_DIR, static_url_path="")
app.secret_key = os.environ.get("SECRET_KEY", "dev-secret-change-in-production")
app.config["MAX_CONTENT_LENGTH"] = 8 * 1024 * 1024  # 8 MB max upload

# Permissive CORS so the frontend (even opened via file://) can call the API
# with session cookies.
@app.after_request
def add_cors(resp):
    origin = request.headers.get("Origin", "*")
    resp.headers["Access-Control-Allow-Origin"] = origin
    resp.headers["Access-Control-Allow-Credentials"] = "true"
    resp.headers["Access-Control-Allow-Headers"] = "Content-Type"
    resp.headers["Access-Control-Allow-Methods"] = "GET, POST, PUT, DELETE, OPTIONS"
    return resp


@app.before_request
def handle_options_preflight():
    if request.method == "OPTIONS":
        return "", 200

ALLOWED_EXTENSIONS = {"jpg", "jpeg", "png", "webp"}


def allowed_file(filename):
    return "." in filename and filename.rsplit(".", 1)[1].lower() in ALLOWED_EXTENSIONS


def safe_filename(original):
    """Generate a server-side safe filename to prevent directory traversal/collisions."""
    ext = original.rsplit(".", 1)[1].lower() if "." in original else ""
    return f"{uuid.uuid4().hex}.{ext}" if ext else f"{uuid.uuid4().hex}"


def login_required(view_fn):
    from functools import wraps

    @wraps(view_fn)
    def wrapper(*args, **kwargs):
        if "user_id" not in session:
            return jsonify({"error": "Authentication required"}), 401
        return view_fn(*args, **kwargs)

    return wrapper


@app.before_request
def init_db():
    Database().create_tables()


# ==================== Auth ====================

@app.route("/api/auth/signup", methods=["POST"])
def signup():
    data = request.get_json(silent=True) or {}
    full_name = (data.get("full_name") or "").strip()
    email = (data.get("email") or "").strip().lower()
    password = data.get("password") or ""

    if not full_name or not email or not password:
        return jsonify({"error": "Full name, email and password are required"}), 400
    if not AuthService.validate_email(email):
        return jsonify({"error": "Invalid email address"}), 400
    if not AuthService.validate_password(password):
        return jsonify({"error": "Password must be at least 6 characters"}), 400

    db = Database()
    if db.get_user_by_email(email):
        return jsonify({"error": "Email already registered"}), 409

    user_id = db.save_user(full_name, email, AuthService.hash_password(password))
    session["user_id"] = user_id
    session["user_name"] = full_name
    session["user_email"] = email
    return (
        jsonify(
            {
                "message": "Account created",
                "user": {"id": user_id, "full_name": full_name, "email": email},
            }
        ),
        201,
    )


@app.route("/api/auth/login", methods=["POST"])
def login():
    data = request.get_json(silent=True) or {}
    email = (data.get("email") or "").strip().lower()
    password = data.get("password") or ""

    if not email or not password:
        return jsonify({"error": "Email and password are required"}), 400

    db = Database()
    user = db.get_user_by_email(email)
    if not user or not AuthService.verify_password(password, user["password_hash"]):
        return jsonify({"error": "Invalid email or password"}), 401

    session["user_id"] = user["id"]
    session["user_name"] = user["full_name"]
    session["user_email"] = user["email"]
    return jsonify(
        {
            "message": "Logged in",
            "user": {
                "id": user["id"],
                "full_name": user["full_name"],
                "email": user["email"],
            },
        }
    )


@app.route("/api/auth/logout", methods=["POST"])
def logout():
    session.clear()
    return jsonify({"message": "Logged out"})


@app.route("/api/auth/me", methods=["GET"])
def me():
    if "user_id" not in session:
        return jsonify({"error": "Not authenticated"}), 401
    return jsonify(
        {
            "user": {
                "id": session["user_id"],
                "full_name": session.get("user_name"),
                "email": session.get("user_email"),
            }
        }
    )


@app.route("/api/auth/change-password", methods=["POST"])
@login_required
def change_password():
    data = request.get_json(silent=True) or {}
    old_password = data.get("old_password") or ""
    new_password = data.get("new_password") or ""
    user_id = session["user_id"]

    if not old_password or not new_password:
        return jsonify({"error": "Old and new password required"}), 400
    if not AuthService.validate_password(new_password):
        return jsonify({"error": "New password must be at least 6 characters"}), 400

    db = Database()
    user = db.get_user_by_email(session.get("user_email", ""))
    if not user or not AuthService.verify_password(old_password, user["password_hash"]):
        return jsonify({"error": "Current password is incorrect"}), 401

    db.update_user_password(user_id, AuthService.hash_password(new_password))
    return jsonify({"message": "Password updated"})


# ==================== Dashboard ====================

@app.route("/api/dashboard", methods=["GET"])
@login_required
def dashboard():
    user_id = session["user_id"]
    d = Dashboard(user_id)
    return jsonify(
        {
            "total": d.get_total_inspections(),
            "good": d.get_good_count(),
            "poor": d.get_poor_count(),
            "needs_inspection": d.get_needs_inspection_count(),
            "category_statistics": d.get_category_statistics(),
            "recent_inspections": d.get_recent_inspections(),
        }
    )


# ==================== Inspection ====================

@app.route("/api/inspect", methods=["POST"])
@login_required
def inspect():
    user_id = session["user_id"]
    if "image" not in request.files:
        return jsonify({"error": "No image file provided"}), 400

    file = request.files["image"]
    if file.filename == "":
        return jsonify({"error": "No image selected"}), 400
    if not allowed_file(file.filename):
        return jsonify({"error": "Unsupported file type (jpg, jpeg, png, webp)"}), 400

    product_name = (request.form.get("product_name") or "").strip()
    category = (request.form.get("category") or "").strip()
    batch_number = (request.form.get("batch_number") or "").strip()
    inspection_location = (request.form.get("inspection_location") or "").strip()

    if not product_name or not category:
        return jsonify({"error": "Product name and category are required"}), 400

    os.makedirs(UPLOADS_DIR, exist_ok=True)
    filename = safe_filename(file.filename)
    save_path = os.path.join(UPLOADS_DIR, filename)
    file.save(save_path)

    # Validate actual image content with PIL
    proc = ImageProcessor()
    if not proc.load_image(save_path):
        try:
            os.remove(save_path)
        except OSError:
            pass
        return jsonify({"error": "Uploaded file is not a valid image"}), 400

    # Polymorphism via the manager's detector
    manager = InspectionManager()
    manager.start_inspection(user_id)
    result = manager.process_image(
        user_id,
        save_path,
        product_name,
        category,
        batch_number,
        inspection_location,
    )

    # Store only the filename (server-side path never exposed to the client).
    db = Database()
    conn = db.connect()
    try:
        conn.execute("UPDATE inspections SET image_path = ? WHERE id = ?", (filename, result["inspection_id"]))
        conn.commit()
    finally:
        conn.close()

    image_url = f"/uploads/{filename}"
    return (
        jsonify(
            {
                "message": "Inspection complete",
                "inspection_id": result["inspection_id"],
                "quality_status": result["quality_status"],
                "confidence": result["confidence"],
                "features": result["features"],
                "analysis_mode": result["analysis_mode"],
                "image_url": image_url,
                "product_name": product_name,
                "category": category,
                "model_based": result["analysis_mode"] != "Basic Visual Analysis",
            }
        ),
        200,
    )


@app.route("/api/inspections", methods=["GET"])
@login_required
def list_inspections():
    user_id = session["user_id"]
    manager = InspectionManager()
    return jsonify(manager.get_history(user_id))


@app.route("/api/inspections/<int:inspection_id>", methods=["GET", "DELETE"])
@login_required
def inspection_detail(inspection_id):
    user_id = session["user_id"]
    db = Database()
    if request.method == "DELETE":
        manager = InspectionManager()
        manager.delete_inspection(inspection_id, user_id)
        return jsonify({"message": "Inspection deleted"})
    rec = db.get_inspection_by_id(inspection_id, user_id)
    if not rec:
        return jsonify({"error": "Inspection not found"}), 404
    return jsonify(rec)


# ==================== Reports ====================

@app.route("/api/reports/inspection/<int:inspection_id>", methods=["GET"])
@login_required
def report_inspection(inspection_id):
    user_id = session["user_id"]
    rg = ReportGenerator()
    rec = rg.generate_single_inspection_report(inspection_id, user_id)
    if not rec:
        return jsonify({"error": "Inspection not found"}), 404
    text = (
        "FreshGuard AI - Single Inspection Report\n"
        "------------------------------------------\n"
        f"Inspection ID : {rec['id']}\n"
        f"Product       : {rec['product_name']}\n"
        f"Category      : {rec['category']}\n"
        f"Quality       : {rec['quality_status']}\n"
        f"Analysis Mode : {rec['analysis_mode']}\n"
        f"Image         : {rec['image_path']}\n"
        f"Batch         : {rec.get('batch_number','')}\n"
        f"Location      : {rec.get('inspection_location','')}\n"
        f"Date          : {rec['created_at']}\n"
        "\nNote: Basic visual-analysis demonstration only.\n"
        "Not a guarantee of internal freshness or food safety.\n"
    )
    return send_file(
        io.BytesIO(text.encode("utf-8")),
        mimetype="text/plain",
        as_attachment=True,
        download_name=f"inspection_{inspection_id}.txt",
    )


@app.route("/api/reports/history.csv", methods=["GET"])
@login_required
def report_history_csv():
    user_id = session["user_id"]
    rg = ReportGenerator()
    csv_data = rg.generate_csv_report(user_id)
    return send_file(
        io.BytesIO(csv_data.encode("utf-8")),
        mimetype="text/csv",
        as_attachment=True,
        download_name="inspection_history.csv",
    )


# ==================== Profile ====================

@app.route("/api/profile", methods=["PUT"])
@login_required
def profile():
    user_id = session["user_id"]
    data = request.get_json(silent=True) or {}
    full_name = (data.get("full_name") or "").strip()
    email = (data.get("email") or "").strip().lower()
    if not full_name or not email:
        return jsonify({"error": "Full name and email required"}), 400
    db = Database()
    db.update_user_profile(user_id, full_name, email)
    session["user_name"] = full_name
    session["user_email"] = email
    return jsonify({"message": "Profile updated", "user": {"full_name": full_name, "email": email}})


# ==================== Contact ====================

@app.route("/api/contact", methods=["POST"])
def contact():
    data = request.get_json(silent=True) or {}
    name = (data.get("name") or "").strip()
    email = (data.get("email") or "").strip()
    subject = (data.get("subject") or "").strip()
    message = (data.get("message") or "").strip()
    if not all([name, email, subject, message]):
        return jsonify({"error": "All fields are required"}), 400
    if not AuthService.validate_email(email):
        return jsonify({"error": "Invalid email address"}), 400
    ContactService().save_message(name, email, subject, message)
    return jsonify({"message": "Message sent"})


# ==================== Static / Frontend ====================

@app.route("/uploads/<path:filename>")
def uploaded_file(filename):
    # secure_filename prevents directory traversal
    return send_from_directory(UPLOADS_DIR, secure_filename(filename))


@app.route("/")
def home():
    return app.send_static_file("index.html")


@app.route("/<path:page>")
def pages(page):
    # Serve any HTML page from frontend/
    if os.path.exists(os.path.join(FRONTEND_DIR, page)):
        return app.send_static_file(page)
    return jsonify({"error": "Not found"}), 404


if __name__ == "__main__":
    os.makedirs(UPLOADS_DIR, exist_ok=True)
    Database().create_tables()
    print("FreshGuard AI backend running on http://127.0.0.1:5000")
    app.run(host="127.0.0.1", port=5000, debug=True)
