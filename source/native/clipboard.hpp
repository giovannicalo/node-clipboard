#pragma once

#include <napi.h>

#ifdef WIN32
	#include <Windows.h>
#endif

#include "image.hpp"

namespace nodeClipboard {

	class Clipboard : public Napi::ObjectWrap<Clipboard> {

		private:

			Napi::ThreadSafeFunction callback;

			std::atomic<bool> isAlive = true;

			void loop();

			std::unique_ptr<std::jthread> thread;

		public:

			Clipboard(const Napi::CallbackInfo& info);

			static Napi::Function initialize(Napi::Env environment);

	};

}
