from database import Database
from quality_detector import BasicQualityDetector, ModelQualityDetector


class InspectionManager:
    """Composition: owns a Database and a QualityDetector and coordinates workflow."""

    def __init__(self):
        self.db = Database()
        self.detector = BasicQualityDetector()  # default; demo uses basic detector

    def start_inspection(self, user_id):
        self.db.create_tables()
        return True

    def process_image(
        self,
        user_id,
        image_path,
        product_name,
        category,
        batch_number="",
        inspection_location="",
    ):
        self.db.create_tables()

        # Polymorphism: call analyze() through the QualityDetector reference.
        quality_status, confidence, features = self.detector.analyze(image_path)
        analysis_mode = self.detector.name

        inspection_id = self.db.save_inspection(
            user_id,
            product_name,
            category,
            image_path,
            quality_status,
            analysis_mode,
            detected_features=features,
            batch_number=batch_number,
            inspection_location=inspection_location,
        )

        return {
            "inspection_id": inspection_id,
            "quality_status": quality_status,
            "confidence": confidence,
            "features": features,
            "analysis_mode": analysis_mode,
        }

    def get_history(self, user_id):
        return self.db.get_inspection_history(user_id)

    def delete_inspection(self, inspection_id, user_id):
        return self.db.delete_inspection(inspection_id, user_id)

    def calculate_statistics(self, user_id):
        return {
            "total": self.db.count(user_id),
            "good": self.db.count(user_id, "Good"),
            "poor": self.db.count(user_id, "Poor"),
            "needs_inspection": self.db.count(user_id, "Needs Inspection"),
        }

    def get_recent_inspections(self, user_id, limit=10):
        return self.db.recent_inspections(user_id, limit)
