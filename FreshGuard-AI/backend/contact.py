from database import Database


class ContactService:
    """Stores contact-form messages in SQLite."""

    def __init__(self):
        self.db = Database()

    def save_message(self, name, email, subject, message):
        if not all([name, email, subject, message]):
            return False
        return self.db.save_contact_message(name, email, subject, message)
