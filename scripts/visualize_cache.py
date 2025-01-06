#!/usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from PIL import Image
import sys

def read_pgm(filename):
    """Read PGM file and return numpy array."""
    with Image.open(filename) as img:
        return np.array(img)

def create_spectrogram(data, output_file):
    """Create a beautiful spectrogram visualization."""
    plt.figure(figsize=(12, 8))
    
    # Create heatmap using seaborn
    sns.heatmap(data, 
                cmap='viridis',  # Use viridis colormap for better visibility
                xticklabels=100, # Show fewer tick labels for clarity
                yticklabels=100,
                cbar_kws={'label': 'Access Frequency'})
    
    plt.title('Cache Access Pattern Spectrogram')
    plt.xlabel('Time')
    plt.ylabel('Memory Address')
    
    # Add annotations for different access patterns
    height = data.shape[0]
    plt.axhline(y=height/4, color='r', linestyle='--', alpha=0.3)
    plt.text(data.shape[1]/20, height/4, 'Sequential Access', 
             rotation=0, color='white')
    
    plt.axhline(y=height/2, color='r', linestyle='--', alpha=0.3)
    plt.text(data.shape[1]/20, height/2, 'Random Access',
             rotation=0, color='white')
    
    plt.axhline(y=3*height/4, color='r', linestyle='--', alpha=0.3)
    plt.text(data.shape[1]/20, 3*height/4, 'Strided Access',
             rotation=0, color='white')
    
    # Save with high DPI for better quality
    plt.savefig(output_file, dpi=300, bbox_inches='tight')
    plt.close()

def main():
    if len(sys.argv) != 2:
        print("Usage: python visualize_cache.py <pgm_file>")
        sys.exit(1)
    
    input_file = sys.argv[1]
    output_file = input_file.rsplit('.', 1)[0] + '_viz.png'
    
    # Read and process data
    data = read_pgm(input_file)
    
    # Create visualization
    create_spectrogram(data, output_file)
    print(f"Created visualization: {output_file}")
    
    # Print some statistics
    print("\nAccess Pattern Analysis:")
    print("-----------------------")
    print(f"Time steps analyzed: {data.shape[1]}")
    print(f"Address space range: {data.shape[0]}")
    print(f"Maximum access frequency: {np.max(data)}")
    print(f"Average access frequency: {np.mean(data):.2f}")
    
    # Identify hot spots
    threshold = np.percentile(data, 90)
    hot_spots = np.where(data > threshold)
    if len(hot_spots[0]) > 0:
        print("\nHot Spots Detected:")
        print("------------------")
        for y, x in zip(*hot_spots):
            print(f"Address: {y}, Time: {x}, Frequency: {data[y,x]}")

if __name__ == "__main__":
    main()
