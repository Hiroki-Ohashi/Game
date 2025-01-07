#pragma once
#include "IScene.h"
#include <TextureManager.h>
#include <Camera.h>
#include <Sprite.h>
#include <PostProcess.h>
#include <Skydome.h>
#include <Json.h>

using namespace Engine;

/// <summary>
/// OverScene.h
/// クリアシーンのヘッダーファイル
/// </summary>

// ClearSceneクラス
class OverScene : public IScene {
public:
	// デストラクタ
	~OverScene();

	// 初期化処理
	void Initialize() override;
	// 更新処理
	void Update() override;
	// 描画処理
	void Draw() override;
	void PostDraw() override;

private:
	// カメラ
	Camera camera_;
	// シングルトン呼び出し
	TextureManager* textureManager_ = TextureManager::GetInstance();
	// postProcess
	std::unique_ptr<PostProcess> postProcess_ = nullptr;

	// Sprite
	std::unique_ptr<Sprite> gekitui_ = nullptr;
	std::unique_ptr<Sprite> sareta_ = nullptr;
	std::unique_ptr<Sprite> sentaku_ = nullptr;
	std::unique_ptr<Sprite> retry_ = nullptr;
	std::unique_ptr<Sprite> title_ = nullptr;

	// json
	std::unique_ptr<Json> json_ = nullptr;
	LevelData* levelData_ = nullptr;

	std::unique_ptr<Json> jsonObject_ = nullptr;
	LevelData* levelDataObject_ = nullptr;

	std::unique_ptr<Model> yuka_ = nullptr;

private:
	// テクスチャ
	uint32_t gekitui;
	uint32_t sareta;

	uint32_t sentaku;
	uint32_t retry;
	uint32_t title;

	uint32_t yuka;
	uint32_t player;

	// param
	EulerTransform transform_;

	// cameraSpeed
	float cameraSpeedX = 0.0001f;
	float cameraSpeedY = 0.0002f;
	float cameraMoveSpeed = 0.000005f;
	const Vector2 kCameraMax = { 0.25f , 0.0f };

	// scene
	uint32_t scenePrev;
	float noiseStrength = 100.0f;

	// タイマー
	uint32_t timer;
	const uint32_t timerSpeed = 1;
	const uint32_t kMaxTimer = 20;
	bool blinking;

	// ビネット
	bool isVignette_;
};
