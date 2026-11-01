import csv
import io
from database import Database


class ReportGenerator:
    """High-level report generation delegating to Database/Inspection."""

    def __init__(self):
        self.db = Database()

    def generate_single_inspection_report(self, inspection_id, user_id):
        rec = self.db.get_inspection_by_id(inspection_id, user_id)
        if not rec:
            return None
        return rec

    def generate_csv_report(self, user_id):
        records = self.db.get_inspection_history(user_id)
        output = io.StringIO()
        writer = csv.writer(output)
        writer.writerow(
            [
                "id",
                "product_name",
                "category",
                "quality_status",
                "analysis_mode",
                "batch_number",
                "inspection_location",
                "created_at",
            ]
        )
        for r in records:
            writer.writerow(
                [
                    r["id"],
                    r["product_name"],
                    r["category"],
                    r["quality_status"],
                    r["analysis_mode"],
                    r.get("batch_number", ""),
                    r.get("inspection_location", ""),
                    r["created_at"],
                ]
            )
        return output.getvalue()

    def generate_summary_report(self, user_id):
        return {
            "total": self.db.count(user_id),
            "good": self.db.count(user_id, "Good"),
            "poor": self.db.count(user_id, "Poor"),
            "needs_inspection": self.db.count(user_id, "Needs Inspection"),
        }
