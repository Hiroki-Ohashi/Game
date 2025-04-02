#pragma once
#include <Windows.h>
#include <cstdint>
#include <string>
#include <format>

#include "Function.h"
#include "Triangle.h"
#include "Sprite.h"
#include "ImGuiManeger.h"
#include "MathFunction.h"
#include "Camera.h"
#include "Sphere.h"
#include "Model.h"
#include "TextureManager.h"
#include "imgui.h"

#include "Particle.h"
#include "IScene.h"
#include "PostProcess.h"
#include <Json.h>
#include <SkyBox/Skydome.h>

using namespace Engine;

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")

/// <summary>
/// TitleScene.h
/// クリアシーンのヘッダーファイル
/// </summary>

// TitleSceneクラス
class TitleScene : public IScene {
public:
	// デストラクタ
	~TitleScene();

	// 初期化処理
	void Initialize() override;
	// 更新処理
	void Update() override;
	// 描画処理
	void Draw() override;
	void PostDraw() override;
private:
	// カメラ
	void CameraShake();
	// UI点滅
	void Blinking();

private:
	// カメラ
	Camera camera_;
	// シングルトン呼び出し
	TextureManager* textureManager_ = TextureManager::GetInstance();

	// sprite
	std::unique_ptr<Sprite> title_ = nullptr;
	std::unique_ptr<Sprite> startLog_ = nullptr;
	std::unique_ptr<Sprite> loadSprite_ = nullptr;

	// postProcess
	std::unique_ptr<PostProcess> postProcess_ = nullptr;

	// json
	std::unique_ptr<Json> json_ = nullptr;
	LevelData* levelData_ = nullptr;

	// 天球
	std::unique_ptr<Skydome> skydome_;
private:
	// Pamameter
	EulerTransform transform;

	// cameraSpeed
	float cameraSpeedX = 0.0001f;
	float cameraSpeedY = 0.0002f;
	float cameraMoveSpeed = 0.000005f;
	const Vector2 kCameraMax = { 0.40f , 0.0f };

	// テクスチャ
	uint32_t title;
	uint32_t start;
	uint32_t white;
	uint32_t load;

	// 点滅タイマー
	uint32_t timer;
	const uint32_t timerSpeed = 1;
	const uint32_t kMaxTimer = 30;
	bool blinking;

	// ビネット
	bool isVignette_;
	bool isLoad_;

	// デバッグ用
	Vector2 pos = {};
	Vector2 scale = {};
};