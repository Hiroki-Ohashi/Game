#pragma once
#include <Windows.h>
#include <cstdint>
#include <d3d12.h>
#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"

class WinApp {
public:

	static WinApp* GetInsTance();

	void Initialize(const wchar_t* title);

	WNDCLASS GetHInstance() { return wc; }
	static HWND GetHwnd() { return hwnd; }
	static const int32_t GetKClientWidth() { return kClientWidth; }
	static const int32_t GetKClientHeight() { return kClientHeight; }
	static inline ID3D12Debug1* GetDebugController() { return debugController; }

private:
	WNDCLASS wc{};

	// クライアント領域のサイズ
	static const int32_t kClientWidth = 1280;
	static const int32_t kClientHeight = 720;

	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	static inline HWND hwnd = nullptr;
	static inline ID3D12Debug1* debugController = nullptr;
};