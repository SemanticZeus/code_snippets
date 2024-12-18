import requests
from bs4 import BeautifulSoup
from PIL import Image
from io import BytesIO
import os
import random
import urllib.parse

def fetch_taylor_swift_images(query="Taylor Swift", count=100, min_size=800):
    # Prepare the search URL with URL-encoded query
    url = f"https://www.bing.com/images/search?q={urllib.parse.quote(query)}&FORM=HDRSC2"
    headers = {
        "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/85.0.4183.121 Safari/537.36"
    }

    # Send a GET request to fetch the HTML content of the page
    response = requests.get(url, headers=headers)
    response.raise_for_status()  # Check for HTTP request errors

    # Parse HTML with BeautifulSoup
    soup = BeautifulSoup(response.text, "html.parser")
    image_elements = soup.find_all("img", {"class": "mimg"})  # Bing image class

    # Collect image URLs
    image_urls = []
    for img in image_elements:
        img_url = img.get("src") or img.get("data-src")
        if img_url and img_url.startswith("http"):
            image_urls.append(img_url)

    # Randomly select images, filter by minimum size, and save
    valid_images = []
    for img_url in image_urls:
        try:
            # Download image temporarily to check dimensions
            img_response = requests.get(img_url, stream=True)
            img = Image.open(BytesIO(img_response.content))
            width, height = img.size

            # Only save if it meets the minimum size requirement
            if width >= min_size or height >= min_size:
                valid_images.append(img_url)
                print(f"Image accepted (size: {width}x{height}): {img_url}")

                # Stop if we reach the required count
                if len(valid_images) >= count:
                    break
            else:
                print(f"Image skipped (size: {width}x{height}): {img_url}")

        except Exception as e:
            print(f"Error processing image {img_url}: {e}")

    return valid_images

# Fetch and print valid image URLs
images = fetch_taylor_swift_images()
for idx, image_url in enumerate(images, start=1):
    print(f"{idx}: {image_url}")

