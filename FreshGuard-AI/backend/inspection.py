from datetime import datetime


class Inspection:
    """Encapsulated single quality-inspection record."""

    def __init__(
        self,
        inspection_id=-1,
        user_id=-1,
        product_name="",
        category="",
        image_path="",
        quality_status="",
        inspection_date="",
        analysis_mode="",
    ):
        self._inspection_id = inspection_id
        self._user_id = user_id
        self._product_name = product_name
        self._category = category
        self._image_path = image_path
        self._quality_status = quality_status
        self._inspection_date = inspection_date or datetime.now().strftime(
            "%Y-%m-%d %H:%M:%S"
        )
        self._analysis_mode = analysis_mode

    # getters (encapsulation)
    @property
    def id(self):
        return self._inspection_id

    @property
    def user_id(self):
        return self._user_id

    @property
    def product_name(self):
        return self._product_name

    @property
    def category(self):
        return self._category

    @property
    def image_path(self):
        return self._image_path

    @property
    def quality_status(self):
        return self._quality_status

    @property
    def inspection_date(self):
        return self._inspection_date

    @property
    def analysis_mode(self):
        return self._analysis_mode

    @quality_status.setter
    def quality_status(self, value):
        self._quality_status = value

    def to_dict(self):
        return {
            "id": self._inspection_id,
            "user_id": self._user_id,
            "product_name": self._product_name,
            "category": self._category,
            "image_path": self._image_path,
            "quality_status": self._quality_status,
            "inspection_date": self._inspection_date,
            "analysis_mode": self._analysis_mode,
        }

    def display_result(self):
        print(f"Inspection ID : {self._inspection_id}")
        print(f"Product       : {self._product_name}")
        print(f"Category      : {self._category}")
        print(f"Quality       : {self._quality_status}")
        print(f"Mode          : {self._analysis_mode}")
        print(f"Date          : {self._inspection_date}")

    def generate_report(self):
        lines = [
            "FreshGuard AI - Inspection Report",
            "---------------------------------",
            f"Inspection ID : {self._inspection_id}",
            f"Product       : {self._product_name}",
            f"Category      : {self._category}",
            f"Quality       : {self._quality_status}",
            f"Analysis Mode : {self._analysis_mode}",
            f"Date          : {self._inspection_date}",
            f"Image         : {self._image_path}",
            "",
            "Note: Basic visual analysis demonstration only.",
            "Not a guarantee of internal freshness or food safety.",
        ]
        return "\n".join(lines)
