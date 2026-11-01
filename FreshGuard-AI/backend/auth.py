from werkzeug.security import generate_password_hash, check_password_hash


class AuthService:
    """Authentication and password security.

    Passwords are hashed with Werkzeug (PBKDF2-HMAC-SHA256 by default) and are
    never stored in plain text.
    """

    @staticmethod
    def hash_password(plain_password):
        return generate_password_hash(plain_password)

    @staticmethod
    def verify_password(plain_password, password_hash):
        return check_password_hash(password_hash, plain_password)

    @staticmethod
    def validate_email(email):
        return bool(email) and "@" in email and "." in email.split("@")[-1]

    @staticmethod
    def validate_password(password):
        return bool(password) and len(password) >= 6
