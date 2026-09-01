#include <mutex>
#include <thread>

#include <napi.h>

#ifdef _WIN32
	#define NOMINMAX
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
#endif

#include "image.hpp"

#include "clipboard.hpp"

namespace nodeClipboard {

	Clipboard::Clipboard(const Napi::CallbackInfo& info) :
		Napi::ObjectWrap<Clipboard>(info)
	{
		#ifndef _WIN32
			Napi::Error::New(
				info.Env(),
				"[nodeClipboard::Clipboard::Clipboard] Platform not supported"
			).ThrowAsJavaScriptException();
			return;
		#endif
		if (!info.Length()) {
			Napi::Error::New(
				info.Env(),
				"[nodeClipboard::Clipboard::Clipboard] One argument is required"
			).ThrowAsJavaScriptException();
			return;
		}
		if (!info[0].IsFunction()) {
			Napi::Error::New(
				info.Env(),
				"[nodeClipboard::Clipboard::Clipboard] Argument is not a function"
			).ThrowAsJavaScriptException();
			return;
		}
		callback = Napi::ThreadSafeFunction::New(
			info.Env(),
			info[0].As<Napi::Function>(),
			"data",
			0,
			1
		);
		thread = std::jthread(&Clipboard::loop, this);
		std::unique_lock lock(mutex);
		condition.wait(lock, [this]() {
			return id;
		});
	}

	Napi::Function Clipboard::initialize(Napi::Env environment) {
		auto function = DefineClass(environment, "Clipboard", {
			InstanceMethod("stop", &Clipboard::stop)
		});
		auto constructor = new Napi::FunctionReference();
		*constructor = Napi::Persistent(function);
		environment.SetInstanceData<Napi::FunctionReference>(constructor);
		return function;
	}

	void Clipboard::loop() {
		#ifdef _WIN32
			{
				std::scoped_lock lock(mutex);
				id = GetCurrentThreadId();
			}
			condition.notify_one();
			WNDCLASS windowClass {};
			windowClass.lpfnWndProc = DefWindowProc;
			windowClass.lpszClassName = "Clipboard";
			if (!RegisterClass(&windowClass)) {
				// TODO: Throw error
				return;
			}
			auto window = CreateWindowEx(
				0,
				"Clipboard",
				"",
				0,
				0,
				0,
				0,
				0,
				nullptr,
				nullptr,
				nullptr,
				nullptr
			);
			if (!window) {
				// TODO: Throw error
				return;
			}
			if (!AddClipboardFormatListener(window)) {
				// TODO: Throw error
				return;
			}
			MSG message {};
			callback.Acquire();
			while (GetMessage(&message, nullptr, 0, 0)) {
				if (message.message != WM_CLIPBOARDUPDATE) {
					continue;
				}
				if (!OpenClipboard(window)) {
					// TODO: Throw error
					return;
				}
				UINT format = 0;
				do {
					if (format != CF_DIB && format != CF_UNICODETEXT) {
						continue;
					}
					auto address = GetClipboardData(format);
					if (!address) {
						// TODO: Throw error
						return;
					}
					auto data = GlobalLock(address);
					Image* image = nullptr;
					if (format == CF_DIB) {
						image = new Image(data);
					}
					callback.BlockingCall([data, image](Napi::Env environment, Napi::Function callback) {
						Napi::Value value;
						if (image) {
							value = Napi::Object::New(environment);
							auto object = value.As<Napi::Object>();
							object["data"] = Napi::Uint8Array::New(
								environment,
								image->getSize(),
								Napi::ArrayBuffer::New(
									environment,
									image->getData(),
									image->getSize(),
									[](Napi::Env environment, void* data, void* image) {
										delete static_cast<Image*>(image);
									},
									image
								),
								0,
								napi_uint8_clamped_array
							);
							object["height"] = Napi::Number::New(environment, image->getHeight());
							object["width"] = Napi::Number::New(environment, image->getWidth());
						} else {
							value = Napi::String::New(environment, static_cast<char16_t*>(data));
						}
						callback.Call({ value });
						GlobalUnlock(data);
					});
					break;
				} while (format = EnumClipboardFormats(format));
				CloseClipboard();
			}
			RemoveClipboardFormatListener(window);
			DestroyWindow(window);
			UnregisterClass("Clipboard", nullptr);
			callback.Release();
		#endif
	}

	void Clipboard::stop(const Napi::CallbackInfo& info) {
		#ifdef _WIN32
			PostThreadMessage(id, WM_QUIT, 0, 0);
		#endif
		if (thread.joinable()) {
			thread.join();
		}
		callback.Release();
	}

}
