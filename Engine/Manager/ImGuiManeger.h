#pragma once
#include <Windows.h>
#include <cstdint>
#include <d3d12.h>

class WinApp;

// <summary>
/// ImGuiManager.h
/// ImGui生成のヘッダーファイル
/// </summary>

class ImGuiManeger {
public:
	// シングルトン化
	static ImGuiManeger* GetInstance();

	// 初期化処理
	void Initialize();
	// 更新処理
	void Update();
	// 描画処理
	void Draw();
	// 解放処理
	void Release();
};