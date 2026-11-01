# FreshGuard AI

Fruit and Vegetable Quality Detection System — a C++ OOP academic mini-project with a dark-themed vanilla-JS frontend.

## Project structure
```
FreshGuard-AI/
├── frontend/          # HTML, CSS, JS (vanilla)
├── backend/           # Python/Flask server (runs immediately)
├── backend_cpp/       # Reference C++17 OOP implementation (academic)
├── database/          # schema.sql
├── uploads/           # uploaded images (created at runtime)
├── CMakeLists.txt
├── requirements.txt
├── README.md
└── .gitignore
```

## Why two backends?
The assignment requires C++ OOP, so complete C++17 classes live in `backend_cpp/`. Because this machine has no C++ compiler, a functionally identical Python/Flask backend in `backend/` runs the website end-to-end. Both expose the same API routes.

## Quick run (Python/Flask backend)

1. Install Python 3.10+.
2. Install dependencies:
   ```
   python -m pip install -r requirements.txt
   ```
3. Start the server:
   ```
   cd backend
   python app.py
   ```
   Server: `http://127.0.0.1:5000`
4. Open `frontend/index.html` in your browser (or `http://127.0.0.1:5000/`).
5. Register an account (Signup), then log in.
6. Upload a fruit/vegetable image (JPG/PNG/WEBP, max 8 MB).
7. View the inspection result, history, dashboard stats, and download CSV reports.

## API routes
- `POST /api/auth/signup` — register
- `POST /api/auth/login` — login (session cookie)
- `POST /api/auth/logout` — logout
- `GET /api/auth/me` — current user
- `POST /api/auth/change-password` — change password
- `GET /api/dashboard` — statistics
- `POST /api/inspect` — multipart image upload + analysis
- `GET /api/inspections` — history (current user only)
- `GET /api/inspections/{id}` — single record
- `DELETE /api/inspections/{id}` — delete (owner only)
- `GET /api/reports/inspection/{id}` — single report
- `GET /api/reports/history.csv` — export CSV
- `PUT /api/profile` — update profile
- `POST /api/contact` — contact message

## C++ OOP backend (backend_cpp/)
Classes: User, Database, ImageProcessor, QualityDetector (abstract), BasicQualityDetector,
ModelQualityDetector, Inspection, Report, AuthService, InspectionManager, Dashboard,
ReportGenerator, ContactService.

OOP shown: encapsulation (private data), inheritance (detectors), polymorphism
(analyze via base pointer), abstraction (pure virtual), constructors/destructors,
composition (InspectionManager owns Database + detector), STL (vector/string/map).

To build the C++ version, install a C++17 compiler, Crow, SQLite3 dev, and OpenCV
(optional), then run cmake/make per CMakeLists.txt.

## Security
- Passwords hashed with Werkzeug (PBKDF2-HMAC-SHA256); never plain text.
- Parameterized SQL queries.
- Per-user inspection access (user_id enforced).
- Safe server-side filenames; file type + size validation.

## Note on analysis
Results come from a basic visual-analysis demonstration (color, brightness, variation).
No trained ML model is bundled. Not a food-safety or internal-freshness test.
