import os
import sqlite3
from datetime import datetime

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT_ROOT = os.path.dirname(BASE_DIR)
DB_PATH = os.path.join(PROJECT_ROOT, "database", "freshguard.db")


def _connect():
    os.makedirs(os.path.dirname(DB_PATH), exist_ok=True)
    conn = sqlite3.connect(DB_PATH)
    conn.row_factory = sqlite3.Row
    conn.execute("PRAGMA foreign_keys = ON")
    return conn


class Database:
    """SQLite wrapper using parameterized queries (prevents SQL injection)."""

    def __init__(self, db_path=DB_PATH):
        self.db_path = db_path

    def connect(self):
        return _connect()

    def create_tables(self):
        conn = self.connect()
        try:
            conn.executescript(
                """
                CREATE TABLE IF NOT EXISTS users (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    full_name TEXT NOT NULL,
                    email TEXT UNIQUE NOT NULL,
                    password_hash TEXT NOT NULL,
                    created_at TEXT DEFAULT CURRENT_TIMESTAMP
                );
                CREATE TABLE IF NOT EXISTS inspections (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    user_id INTEGER NOT NULL,
                    product_name TEXT NOT NULL,
                    category TEXT NOT NULL,
                    image_path TEXT NOT NULL,
                    quality_status TEXT NOT NULL,
                    analysis_mode TEXT NOT NULL,
                    detected_features TEXT,
                    batch_number TEXT,
                    inspection_location TEXT,
                    created_at TEXT DEFAULT CURRENT_TIMESTAMP,
                    FOREIGN KEY(user_id) REFERENCES users(id)
                );
                CREATE TABLE IF NOT EXISTS contact_messages (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    name TEXT NOT NULL,
                    email TEXT NOT NULL,
                    subject TEXT NOT NULL,
                    message TEXT NOT NULL,
                    created_at TEXT DEFAULT CURRENT_TIMESTAMP
                );
                CREATE INDEX IF NOT EXISTS idx_inspections_user ON inspections(user_id);
                CREATE INDEX IF NOT EXISTS idx_inspections_created ON inspections(created_at);
                """
            )
            conn.commit()
        finally:
            conn.close()

    # ---------- Users ----------
    def get_user_by_email(self, email):
        conn = self.connect()
        try:
            cur = conn.execute(
                "SELECT id, full_name, email, password_hash FROM users WHERE email = ?",
                (email,),
            )
            return cur.fetchone()
        finally:
            conn.close()

    def save_user(self, full_name, email, password_hash):
        conn = self.connect()
        try:
            cur = conn.execute(
                "INSERT INTO users(full_name, email, password_hash) VALUES(?,?,?)",
                (full_name, email, password_hash),
            )
            conn.commit()
            return cur.lastrowid
        finally:
            conn.close()

    def update_user_profile(self, user_id, full_name, email):
        conn = self.connect()
        try:
            conn.execute(
                "UPDATE users SET full_name = ?, email = ? WHERE id = ?",
                (full_name, email, user_id),
            )
            conn.commit()
            return True
        finally:
            conn.close()

    def update_user_password(self, user_id, password_hash):
        conn = self.connect()
        try:
            conn.execute(
                "UPDATE users SET password_hash = ? WHERE id = ?",
                (password_hash, user_id),
            )
            conn.commit()
            return True
        finally:
            conn.close()

    # ---------- Inspections ----------
    def save_inspection(
        self,
        user_id,
        product_name,
        category,
        image_path,
        quality_status,
        analysis_mode,
        detected_features="",
        batch_number="",
        inspection_location="",
    ):
        conn = self.connect()
        try:
            cur = conn.execute(
                """INSERT INTO inspections
                   (user_id, product_name, category, image_path, quality_status,
                    analysis_mode, detected_features, batch_number, inspection_location)
                   VALUES(?,?,?,?,?,?,?,?,?)""",
                (
                    user_id,
                    product_name,
                    category,
                    image_path,
                    quality_status,
                    analysis_mode,
                    detected_features,
                    batch_number,
                    inspection_location,
                ),
            )
            conn.commit()
            return cur.lastrowid
        finally:
            conn.close()

    def get_inspection_history(self, user_id):
        conn = self.connect()
        try:
            cur = conn.execute(
                """SELECT id, product_name, category, quality_status, analysis_mode,
                          image_path, batch_number, inspection_location, created_at
                   FROM inspections WHERE user_id = ? ORDER BY created_at DESC""",
                (user_id,),
            )
            return [dict(r) for r in cur.fetchall()]
        finally:
            conn.close()

    def get_inspection_by_id(self, inspection_id, user_id):
        conn = self.connect()
        try:
            cur = conn.execute(
                """SELECT id, product_name, category, quality_status, analysis_mode,
                          image_path, detected_features, batch_number,
                          inspection_location, created_at
                   FROM inspections WHERE id = ? AND user_id = ?""",
                (inspection_id, user_id),
            )
            row = cur.fetchone()
            return dict(row) if row else None
        finally:
            conn.close()

    def delete_inspection(self, inspection_id, user_id):
        conn = self.connect()
        try:
            conn.execute(
                "DELETE FROM inspections WHERE id = ? AND user_id = ?",
                (inspection_id, user_id),
            )
            conn.commit()
            return True
        finally:
            conn.close()

    # ---------- Statistics ----------
    def count(self, user_id, status=None):
        conn = self.connect()
        try:
            if status:
                cur = conn.execute(
                    "SELECT COUNT(*) FROM inspections WHERE user_id = ? AND quality_status = ?",
                    (user_id, status),
                )
            else:
                cur = conn.execute(
                    "SELECT COUNT(*) FROM inspections WHERE user_id = ?", (user_id,)
                )
            return cur.fetchone()[0]
        finally:
            conn.close()

    def category_statistics(self, user_id):
        conn = self.connect()
        try:
            cur = conn.execute(
                "SELECT category, COUNT(*) as cnt FROM inspections WHERE user_id = ? GROUP BY category",
                (user_id,),
            )
            return [{"category": r[0], "count": r[1]} for r in cur.fetchall()]
        finally:
            conn.close()

    def recent_inspections(self, user_id, limit=10):
        conn = self.connect()
        try:
            cur = conn.execute(
                """SELECT id, product_name, category, quality_status, created_at
                   FROM inspections WHERE user_id = ? ORDER BY created_at DESC LIMIT ?""",
                (user_id, limit),
            )
            return [dict(r) for r in cur.fetchall()]
        finally:
            conn.close()

    # ---------- Contact ----------
    def save_contact_message(self, name, email, subject, message):
        conn = self.connect()
        try:
            conn.execute(
                "INSERT INTO contact_messages(name, email, subject, message) VALUES(?,?,?,?)",
                (name, email, subject, message),
            )
            conn.commit()
            return True
        finally:
            conn.close()
