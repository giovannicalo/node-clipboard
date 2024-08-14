#include "image.hpp"

namespace nodeClipboard {

	Image::Image(void* bitmap) {
		#ifdef WIN32
			BITMAPINFOHEADER header = static_cast<BITMAPINFO*>(bitmap)->bmiHeader;
			uint8_t* inputData = static_cast<uint8_t*>(bitmap) + sizeof(BITMAPINFOHEADER) + (
				header.biCompression == BI_BITFIELDS ? 3 * sizeof(RGBQUAD) : 0
			);
			height = header.biHeight;
			width = header.biWidth;
			size = height * width * 4;
			data = new uint8_t[size];
			for (uint16_t y = 0; y < height; y++) {
				for (uint16_t x = 0; x < width; x += 8) {
					__m256i mask = _mm256_cmpgt_epi32(
						_mm256_set1_epi32(width),
						_mm256_add_epi32(
							_mm256_set1_epi32(x),
							_mm256_set_epi32(7, 6, 5, 4, 3, 2, 1, 0)
						)
					);
					_mm256_maskstore_epi32(
						reinterpret_cast<int32_t*>(&data[width * 4 * y + x * 4]),
						mask,
						_mm256_shuffle_epi8(
							_mm256_maskload_epi32(
								reinterpret_cast<int32_t*>(&(inputData + width * 4 * (height - 1 - y))[x * 4]),
								mask
							),
							_mm256_set_epi8(
								15, 12, 13, 14, 11, 8, 9, 10,
								7, 4, 5, 6, 3, 0, 1, 2,
								15, 12, 13, 14, 11, 8, 9, 10,
								7, 4, 5, 6, 3, 0, 1, 2
							)
						)
					);
				}
			}
		#endif
	}

	Image::~Image() {
		delete[] data;
	}

	uint8_t* Image::getData() const {
		return data;
	}

	uint16_t Image::getHeight() const {
		return height;
	}

	uint32_t Image::getSize() const {
		return size;
	}

	uint16_t Image::getWidth() const {
		return width;
	}

}
