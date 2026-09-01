#pragma once

#include <condition_variable>
#include <mutex>

#include <napi.h>

#ifdef _WIN32
	#define NOMINMAX
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
#endif

namespace nodeClipboard {

	class Clipboard : public Napi::ObjectWrap<Clipboard> {

		private:

			Napi::ThreadSafeFunction callback;

			std::condition_variable condition;

			#ifdef _WIN32
				DWORD id = 0;
			#endif

			void loop();

			std::mutex mutex;

			std::jthread thread;

		public:

			Clipboard(const Napi::CallbackInfo& info);

			static Napi::Function initialize(Napi::Env environment);

			void stop(const Napi::CallbackInfo& info);

	};

}
