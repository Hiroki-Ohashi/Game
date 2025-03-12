#pragma once
#include "IScene.h"
#include <TextureManager.h>
#include <Camera.h>
#include <Sprite.h>
#include <PostProcess.h>
#include <Json.h>
#include <SkyBox/Skydome.h>
using namespace Engine;

/// <summary>
/// ClearScene.h
/// クリアシーンのヘッダーファイル
/// </summary>

// ClearSceneクラス
class ClearScene : public IScene {
public:
	// デストラクタ
	~ClearScene();

	// 初期化処理
	void Initialize() override;
	// 更新処理
	void Update() override;
	// 描画処理
	void Draw() override;
	void PostDraw() override;

private:
	// カメラ
	void CameraMove();
	// UI点滅
	void Blinking();


private:
	// カメラ
	Camera camera_;
	// シングルトン呼び出し
	TextureManager* textureManager_ = TextureManager::GetInstance();

	// sprite
	std::unique_ptr<Sprite> clear_ = nullptr;
	std::unique_ptr<Sprite> clearLog_ = nullptr;
	std::unique_ptr<Sprite> log_ = nullptr;

	// postProcess
	std::unique_ptr<PostProcess> postProcess_ = nullptr;

	// 天球
	std::unique_ptr<Skydome> skydome_;

	// json
	std::unique_ptr<Json> json_ = nullptr;
	LevelData* levelData_ = nullptr;

	// Pamameter
	EulerTransform transform;

	// テクスチャ
	uint32_t clear;
	uint32_t clearLog;
	uint32_t log;
	uint32_t player;

	// タイマー
	uint32_t timer;
	const uint32_t timerSpeed = 1;
	const uint32_t kMaxTimer = 30;
	bool blinking;

	// ビネット
	bool isVignette_;
	float noiseStrength = 0.0f;

	// デバッグ用
	Vector2 pos = {};
	Vector2 scale = {};
};