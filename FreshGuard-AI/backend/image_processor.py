from PIL import Image


class ImageProcessor:
    """Basic visual image processing using Pillow.

    load_image, resize_image, calculate_average_color, calculate_brightness,
    calculate_color_variation mirror the C++ ImageProcessor class.
    """

    def __init__(self, image_path=None):
        self.image_path = image_path
        self.image = None
        self.width = 0
        self.height = 0
        self.channels = 0
        if image_path:
            self.load_image(image_path)

    def load_image(self, path):
        try:
            self.image = Image.open(path).convert("RGB")
            self.image_path = path
            self.width, self.height = self.image.size
            self.channels = 3
            return True
        except Exception:
            self.image = None
            return False

    def resize_image(self, target_width, target_height):
        if not self.image:
            return False
        self.image = self.image.resize((target_width, target_height))
        self.width, self.height = self.image.size
        return True

    def calculate_average_color(self):
        if not self.image:
            return (0, 0, 0)
        small = self.image.resize((50, 50))
        pixels = list(small.getdata())
        n = len(pixels)
        r = sum(p[0] for p in pixels) // n
        g = sum(p[1] for p in pixels) // n
        b = sum(p[2] for p in pixels) // n
        return (r, g, b)

    def calculate_brightness(self):
        if not self.image:
            return 0.0
        r, g, b = self.calculate_average_color()
        # Perceived luminance formula
        return (0.299 * r + 0.587 * g + 0.114 * b) / 255.0

    def calculate_color_variation(self):
        if not self.image:
            return 0.0
        small = self.image.resize((40, 40))
        pixels = list(small.getdata())
        r_avg = sum(p[0] for p in pixels) / len(pixels)
        g_avg = sum(p[1] for p in pixels) / len(pixels)
        b_avg = sum(p[2] for p in pixels) / len(pixels)
        var = sum(
            (p[0] - r_avg) ** 2 + (p[1] - g_avg) ** 2 + (p[2] - b_avg) ** 2
            for p in pixels
        ) / len(pixels)
        return min(var / (255.0 * 255.0), 1.0)
