

# Define a simple 256-color palette (indexed by values 0-255)
palette = [
    (255, 0, 0),      # 0: Red
    (0, 255, 0),      # 1: Green
    (0, 0, 255),      # 2: Blue
    # Add more colors as needed...
    # For simplicity, let's fill in all 256 colors with a pattern
]

# Populate the rest of the palette with some generated colors for demonstration
# (e.g., gradient of RGB values)
for i in range(3, 256):
    palette.append((i, 255-i, (i*2) % 256))

# Function to map a byte (0-255) to a color in the palette
def byte_to_color(byte_value):
    if 0 <= byte_value < 256:
        return palette[byte_value]
    else:
        raise ValueError("Byte value must be between 0 and 255")

# Test the function with different byte values
print(byte_to_color(0))   # Output: (255, 0, 0) - Red
print(byte_to_color(1))   # Output: (0, 255, 0) - Green
print(byte_to_color(2))   # Output: (0, 0, 255) - Blue
print(byte_to_color(255)) # Output: (255, 0, 254) - Example color

# Now you can use byte_to_color(byte_value) to map a byte to its color!
