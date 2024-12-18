#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <ranges>
#include <algorithm>
#include <iomanip>

// Simple BMP Header
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

// Load BMP File
std::vector<uint8_t> loadBMP(const std::string& filename, BMPHeader& header) {
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

// Perform Discrete Cosine Transform (DCT) on 8x8 block
void dctBlock(const std::vector<uint8_t>& input, std::vector<double>& output, int width, int x, int y) {
    constexpr int blockSize = 8;
    constexpr double PI = 3.14159265358979323846;

    for (int v = 0; v < blockSize; ++v) {
        for (int u = 0; u < blockSize; ++u) {
            double sum = 0.0;
            for (int j = 0; j < blockSize; ++j) {
                for (int i = 0; i < blockSize; ++i) {
                    int px = std::min(x + i, width - 1);
                    int py = std::min(y + j, width - 1);
                    double pixelValue = static_cast<double>(input[py * width + px]);
                    sum += pixelValue * std::cos((2 * i + 1) * u * PI / (2 * blockSize)) *
                           std::cos((2 * j + 1) * v * PI / (2 * blockSize));
                }
            }
            double alphaU = (u == 0) ? 1.0 / std::sqrt(2.0) : 1.0;
            double alphaV = (v == 0) ? 1.0 / std::sqrt(2.0) : 1.0;
            output[v * blockSize + u] = 0.25 * alphaU * alphaV * sum;
        }
    }
}

// Quantize DCT Coefficients
void quantizeBlock(std::vector<double>& block) {
    constexpr int blockSize = 8;
    constexpr int quantMatrix[blockSize][blockSize] = {
        {16, 11, 10, 16, 24, 40, 51, 61},
        {12, 12, 14, 19, 26, 58, 60, 55},
        {14, 13, 16, 24, 40, 57, 69, 56},
        {14, 17, 22, 29, 51, 87, 80, 62},
        {18, 22, 37, 56, 68, 109, 103, 77},
        {24, 35, 55, 64, 81, 104, 113, 92},
        {49, 64, 78, 87, 103, 121, 120, 101},
        {72, 92, 95, 98, 112, 100, 103, 99},
    };

    for (int v = 0; v < blockSize; ++v) {
        for (int u = 0; u < blockSize; ++u) {
            block[v * blockSize + u] = std::round(block[v * blockSize + u] / quantMatrix[v][u]);
        }
    }
}

// Compress Image
void compressImage(const std::vector<uint8_t>& input, std::vector<uint8_t>& output, int width, int height) {
    constexpr int blockSize = 8;
    std::vector<double> dctBlockData(blockSize * blockSize);

    for (int y = 0; y < height; y += blockSize) {
        for (int x = 0; x < width; x += blockSize) {
            // Perform DCT on the block
            dctBlock(input, dctBlockData, width, x, y);

            // Quantize the DCT coefficients
            quantizeBlock(dctBlockData);

            // Store quantized coefficients in the output
            for (double coeff : dctBlockData) {
                output.push_back(static_cast<uint8_t>(std::clamp(coeff, 0.0, 255.0)));
            }
        }
    }
}

// Save JPEG-Like File
void saveJPEG(const std::string& filename, const std::vector<uint8_t>& compressedData, int width, int height) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open output file.");
    }

    // Write simple header
    file.write("JPEG-LIKE", 9);
    file.write(reinterpret_cast<const char*>(&width), sizeof(width));
    file.write(reinterpret_cast<const char*>(&height), sizeof(height));

    // Write compressed data
    file.write(reinterpret_cast<const char*>(compressedData.data()), compressedData.size());
    file.close();
}

// Main Function
int main() {
    try {
        const std::string inputBMP = "033.bmp";
        const std::string outputJPEG = "output.jpg";

        BMPHeader header;
        auto pixelData = loadBMP(inputBMP, header);

        if (header.bitCount != 8) {
            throw std::runtime_error("Only 8-bit grayscale BMP files are supported.");
        }

        std::vector<uint8_t> compressedData;
        compressImage(pixelData, compressedData, header.width, header.height);

        saveJPEG(outputJPEG, compressedData, header.width, header.height);

        std::cout << "BMP to JPEG-like conversion completed: " << outputJPEG << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

