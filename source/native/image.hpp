#pragma once

#include <cstdint>

#ifdef WIN32
	#include <Windows.h>
	#include <immintrin.h>
#endif

namespace nodeClipboard {

	class Image {

		private:

			uint8_t* data = nullptr;

			uint16_t height = 0;

			uint32_t size = 0;

			uint16_t width = 0;

		public:

			Image(void* bitmap);

			~Image();

			uint8_t* getData() const;

			uint16_t getHeight() const;

			uint32_t getSize() const;

			uint16_t getWidth() const;

	};

}
