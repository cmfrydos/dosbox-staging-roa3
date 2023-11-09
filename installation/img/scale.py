from PIL import Image
import os

def scale_and_center_image(input_image_path, output_image_path, target_size, small_size=True):
    original_image = Image.open(input_image_path)
    original_width, original_height = original_image.size

    # Calculate the scale factor while keeping the aspect ratio
    scale_factor = min(target_size[0] / original_width, target_size[1] / original_height)

    # For small sizes, reduce the scale factor by 15% for margin
    
    scale_factor *= 0.85

    # Calculate the new size
    new_width = int(original_width * scale_factor)
    new_height = int(original_height * scale_factor)

    # Resize the image
    resized_image = original_image.resize((new_width, new_height), Image.ANTIALIAS)

    # Create a new image with white background
    new_image = Image.new("RGB", target_size, (255, 255, 255))
    
    # Calculate position to paste the resized image
    if small_size:
        # Center the image with margins for small sizes
        x = (target_size[0] - new_width) // 2
        y = (target_size[1] - new_height) // 2
    else:
        # Center the image with margins for small sizes
        x = (target_size[0] - new_width) // 2
        y = x #(target_size[1] - new_height) // 2

    # Paste the resized image onto the white background
    new_image.paste(resized_image, (x, y), resized_image)

    # Save the new image as BMP
    new_image.save(output_image_path, "BMP")

def scale_and_center_gh(input_image_path, output_image_path, target_size):
    original_image = Image.open(input_image_path)
    original_width, original_height = original_image.size

    # Calculate the scale factor while keeping the aspect ratio
    scale_factor = min(target_size[0] / original_width, target_size[1] / original_height)

    # For small sizes, reduce the scale factor by 15% for margin
    
    scale_factor *= 0.8

    # Calculate the new size
    new_width = int(original_width * scale_factor)
    new_height = int(original_height * scale_factor)

    # Resize the image
    resized_image = original_image.resize((new_width, new_height), Image.LANCZOS)

    # Create a new image with white background
    new_image = Image.new("RGBA", target_size, (0, 0, 0, 0))
    

    # Center the image with margins for small sizes
    x = (target_size[0] - new_width) // 2
    y = (target_size[1] - new_height) // 2


    # Paste the resized image onto the white background
    new_image.paste(resized_image, (x, y), resized_image)

    # Save the new image as BMP
    new_image.save(output_image_path, "PNG")

def generate_scaled_images(image_name):
    base_name, _ = os.path.splitext(image_name)
    output_directory = "./"

    # Create the output directory if it doesn't exist
    if not os.path.exists(output_directory):
        os.makedirs(output_directory)

    # Scale for WizardSmallImageFile
    small_sizes = [(55, 55), (64, 68), (83, 80), (92, 97), (110, 106), (119, 123), (138, 140)]
    for size in small_sizes:
        output_name = f"{output_directory}{base_name}_small_{size[0]}x{size[1]}.bmp"
        scale_and_center_image(image_name, output_name, size, small_size=True)

    # Scale for WizardImageFile
    large_sizes = [(164, 314), (192, 386), (246, 459), (273, 556), (328, 604), (355, 700), (410, 797)]
    for size in large_sizes:
        output_name = f"{output_directory}{base_name}_large_{size[0]}x{size[1]}.bmp"
        scale_and_center_image(image_name, output_name, size, small_size=False)

    scale_and_center_gh(image_name, "riva_crystal.png", (640, 360))
# Replace 'orig.png' with the path to your image
generate_scaled_images('orig.png')
