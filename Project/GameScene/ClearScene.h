#pragma once
#include "IScene.h"
#include <TextureManager.h>
#include <Camera.h>
#include <Sprite.h>
#include <PostProcess.h>
#include <Skydome.h>
#include <Json.h>

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
	bool blinking;

	// ビネット
	bool isVignette_;
	float noiseStrength = 0.0f;
};