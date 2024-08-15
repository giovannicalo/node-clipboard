#include <napi.h>

#include "clipboard.hpp"

Napi::Object initialize(Napi::Env environment, Napi::Object exports) {
	exports = nodeClipboard::Clipboard::initialize(environment);
	return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, initialize)
