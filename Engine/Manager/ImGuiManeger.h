#pragma once
#include <Windows.h>
#include <cstdint>
#include <d3d12.h>
#include <WinApp.h>
#include "DirectXCommon.h"

using namespace Engine;

// <summary>
/// ImGuiManager.h
/// ImGui生成のヘッダーファイル
/// </summary>

// ImGuiManagerクラス
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