#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <stdexcept>

// BMP Header structure
#pragma pack(push, 1)
struct BMPHeader {
    uint16_t fileType;       // File type ('BM')
    uint32_t fileSize;       // File size in bytes
    uint16_t reserved1;      // Reserved
    uint16_t reserved2;      // Reserved
    uint32_t offsetData;     // Offset to start of pixel data
    uint32_t size;           // Header size
    int32_t width;           // Image width
    int32_t height;          // Image height
    uint16_t planes;         // Number of color planes
    uint16_t bitCount;       // Bits per pixel
    uint32_t compression;    // Compression type
    uint32_t sizeImage;      // Image size in bytes
    int32_t xPixelsPerMeter; // Horizontal resolution
    int32_t yPixelsPerMeter; // Vertical resolution
    uint32_t colorsUsed;     // Number of colors used
    uint32_t colorsImportant; // Important colors
};
#pragma pack(pop)

// Function to load BMP
std::vector<uint8_t> loadBMP(const std::string& filename, BMPHeader& header) {
  std::cout << filename << std::endl;
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open BMP file.");
    }

    // Read BMP header
    file.read(reinterpret_cast<char*>(&header), sizeof(BMPHeader));
    if (header.fileType != 0x4D42) { // 'BM'
        throw std::runtime_error("Invalid BMP file.");
    }

    // Seek to pixel data
    file.seekg(header.offsetData, std::ios::beg);

    // Read pixel data
    size_t dataSize = header.sizeImage ? header.sizeImage : (header.width * header.height * header.bitCount / 8);
    std::vector<uint8_t> pixelData(dataSize);
    file.read(reinterpret_cast<char*>(pixelData.data()), dataSize);

    return pixelData;
}

// Function to perform FFTW on image blocks
void transformBlocksWithFFTW(std::vector<uint8_t>& pixelData, int width, int height, int blockSize) {
    int blocksX = (width + blockSize - 1) / blockSize;
    int blocksY = (height + blockSize - 1) / blockSize;

    fftw_complex* input = fftw_alloc_complex(blockSize * blockSize);
    fftw_complex* output = fftw_alloc_complex(blockSize * blockSize);
    fftw_plan plan = fftw_plan_dft_2d(blockSize, blockSize, input, output, FFTW_FORWARD, FFTW_ESTIMATE);

    // Process each block
    for (int by = 0; by < blocksY; ++by) {
        for (int bx = 0; bx < blocksX; ++bx) {
            // Fill input with block data
            for (int y = 0; y < blockSize; ++y) {
                for (int x = 0; x < blockSize; ++x) {
                    int px = bx * blockSize + x;
                    int py = by * blockSize + y;
                    if (px < width && py < height) {
                        input[y * blockSize + x][0] = pixelData[py * width + px]; // Real part
                    } else {
                        input[y * blockSize + x][0] = 0; // Padding
                    }
                    input[y * blockSize + x][1] = 0.0; // Imaginary part
                }
            }

            // Perform FFT
            fftw_execute(plan);

            // Quantize and copy output back (simplified)
            for (int y = 0; y < blockSize; ++y) {
                for (int x = 0; x < blockSize; ++x) {
                    int idx = y * blockSize + x;
                    double magnitude = std::sqrt(output[idx][0] * output[idx][0] + output[idx][1] * output[idx][1]);
                    pixelData[(by * blockSize + y) * width + (bx * blockSize + x)] = static_cast<uint8_t>(std::min(255.0, magnitude / 10.0)); // Simple scaling
                }
            }
        }
    }

    fftw_destroy_plan(plan);
    fftw_free(input);
    fftw_free(output);
}

// Function to save JPEG-like output
void saveJPEGLike(const std::string& filename, const std::vector<uint8_t>& pixelData, int width, int height) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open output file.");
    }

    // Write simple header
    file.write("JPEG-LIKE", 9);
    file.write(reinterpret_cast<const char*>(&width), sizeof(width));
    file.write(reinterpret_cast<const char*>(&height), sizeof(height));

    // Write pixel data
    file.write(reinterpret_cast<const char*>(pixelData.data()), pixelData.size());
    file.close();
}

// Main function
int main() {
    try {
        std::string inputBMP = "033.bmp";
        std::string outputJPEGLike = "output.jpg";

        BMPHeader header;
        auto pixelData = loadBMP(inputBMP, header);

        if (header.bitCount != 8) {
            throw std::runtime_error("Only 8-bit BMP files are supported.");
        }

        // Apply FFTW on 8x8 blocks (similar to JPEG DCT blocks)
        transformBlocksWithFFTW(pixelData, header.width, header.height, 8);

        // Save as JPEG-like output
        saveJPEGLike(outputJPEGLike, pixelData, header.width, header.height);

        std::cout << "BMP to JPEG-like conversion completed: " << outputJPEGLike << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

