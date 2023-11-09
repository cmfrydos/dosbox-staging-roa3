from PIL import Image
from PIL import Image, ImageFilter

def sharpen_image(image, level=2.0):
    """
    Apply sharpening to the image.
    :param image: PIL Image object
    :param level: Sharpening level. Higher means more sharpening.
    :return: Sharpened PIL Image object
    """
    sharpener = ImageFilter.UnsharpMask(radius=1, percent=150, threshold=3)
    return image.filter(sharpener)

def resize_image(image, size, resample=Image.LANCZOS):
    """
    Resize the image to a specified size.
    :param image: PIL Image object
    :param size: Tuple of (width, height)
    :param resample: Resampling filter
    :return: Resized PIL Image object
    """
    return image.resize(size, resample=resample)


def bleng(c,a):
    return int(255 * ((c / 255 * a / 255) + 1 - (a / 255)))

def process_image(input_png, output_ico):
    img = Image.open(input_png).convert('RGBA')
    width, height = img.size

    pixels = img.load()
    for y in range(height):
        for x in range(width):
            r, g, b, a = pixels[x, y]
            if a < 128:
                pixels[x, y] = (255, 255, 255, 0)
            else:
                pixels[x, y] = (bleng(r,a), bleng(g,a), bleng(b,a), 255)
    img_rgb = img.convert('P', palette=Image.ADAPTIVE, colors=256)

    resolutions = [16, 24, 32, 48, 64, 72, 80, 96, 128, 256]
    # Create an ICO with different resolutions
    img_rgb.save(output_ico, format='ICO', sizes=[(res, res) for res in resolutions])

    # Example usage
process_image('orig.png', 'riva.ico')
