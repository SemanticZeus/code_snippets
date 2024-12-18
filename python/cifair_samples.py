import tensorflow_datasets as tfds
import matplotlib.pyplot as plt
import numpy as np

# Load the CiFAIR-10 dataset
ds = tfds.load('cifar10', split='train', shuffle_files=True)
ds = ds.shuffle(buffer_size=10000)  # Shuffle for randomness

# Prepare to plot 10 random images
fig, axes = plt.subplots(2, 5, figsize=(10, 5))
fig.suptitle("10 Random Images from the CiFAIR-10 Dataset")

for i, sample in enumerate(ds.take(10)):  # Take 10 samples
    image = sample['image']
    label = sample['label'].numpy()
    
    # Display each image in a grid
    ax = axes[i // 5, i % 5]
    ax.imshow(image)
    ax.axis("off")
    ax.set_title(f"Label: {label}")

plt.show()

