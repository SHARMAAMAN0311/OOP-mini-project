from abc import ABC, abstractmethod
from image_processor import ImageProcessor


class QualityDetector(ABC):
    """Abstract base class (Abstraction). Declares the pure virtual analyze()."""

    @abstractmethod
    def analyze(self, image_path):
        """Return (quality_status, confidence, features)."""
        pass

    @property
    @abstractmethod
    def name(self):
        pass


class BasicQualityDetector(QualityDetector):
    """Inheritance from QualityDetector. Basic visual-analysis demo result."""

    @property
    def name(self):
        return "Basic Visual Analysis"

    def analyze(self, image_path):
        proc = ImageProcessor(image_path)
        if not proc.load_image(image_path):
            return "Needs Inspection", 0, "image could not be read"

        r, g, b = proc.calculate_average_color()
        brightness = proc.calculate_brightness()
        variation = proc.calculate_color_variation()

        # Heuristic demonstration only - NOT a freshness/food-safety test.
        if g > r and g > b and brightness > 0.35:
            status = "Good"
            confidence = 78
        elif variation > 0.5 or brightness < 0.25:
            status = "Poor"
            confidence = 65
        else:
            status = "Needs Inspection"
            confidence = 60

        features = f"avg_color=({r},{g},{b}), brightness={brightness:.2f}, variation={variation:.2f}"
        return status, confidence, features


class ModelQualityDetector(QualityDetector):
    """Inheritance from QualityDetector. Used only when a real model exists."""

    def __init__(self):
        self.available = False  # no trained model bundled

    @property
    def name(self):
        return "Trained Model"

    def analyze(self, image_path):
        # No fabricated predictions. When no model is available, report needs
        # inspection and let the basic detector represent the result.
        return "Needs Inspection", 0, "no trained model available"
