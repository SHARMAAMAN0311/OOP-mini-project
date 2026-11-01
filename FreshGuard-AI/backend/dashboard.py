from database import Database


class Dashboard:
    """Statistics retrieved from actual SQLite records (never hardcoded)."""

    def __init__(self, user_id):
        self.db = Database()
        self.user_id = user_id

    def get_total_inspections(self):
        return self.db.count(self.user_id)

    def get_good_count(self):
        return self.db.count(self.user_id, "Good")

    def get_poor_count(self):
        return self.db.count(self.user_id, "Poor")

    def get_needs_inspection_count(self):
        return self.db.count(self.user_id, "Needs Inspection")

    def get_category_statistics(self):
        return self.db.category_statistics(self.user_id)

    def get_recent_inspections(self, limit=10):
        return self.db.recent_inspections(self.user_id, limit)
