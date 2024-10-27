#pragma once
#include "WinApp.h"
#include "DirectXCommon.h"
#include "Input.h"

#include "IScene.h"
#include "TitleScene.h"
#include "GameScene.h"
#include <ClearScene.h>
#include "OverScene.h"

#include "ImGuiManeger.h"
#include "imgui.h"
#include "Light.h"

/// <summary>
/// GameManager.h
/// Sceneのマネージャーのヘッダーファイル
/// </summary>

// GameManagerクラス
class GameManager {
public:
	// コンストラクタ
	GameManager();
	// デストラクタ
	~GameManager();
	// シングルトン
	static GameManager* GetInsTance();
	// main
	void Run();

private:
	// シングルトン呼び出し
	WinApp* winapp = nullptr;
	DirectXCommon* directX = nullptr;
	Mesh* mesh = nullptr;
	Input* input = nullptr;
	ImGuiManeger* imgui = nullptr;
	Light* light_ = nullptr;


	// シーンを保持
	std::unique_ptr<IScene> sceneArr_[4];

	// シーン管理
	int currentSceneNo_ = TITLE;
	int prvSceneNo_ = 0;
	int sceneNo = 0;

};