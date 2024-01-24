#pragma once
#include <Windows.h>
#include <cstdint>
#include <d3d12.h>

class WinApp;

class ImGuiManeger {
public:
	static ImGuiManeger* GetInstance();

	void Initialize();

	void Update();

	void Draw();

	void Release();
};