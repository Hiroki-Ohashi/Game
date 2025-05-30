#pragma once
#include "IScene.h"
#include <TextureManager.h>
#include <Camera.h>
#include <Sprite.h>
#include <PostProcess.h>
#include <SkyBox/Skydome.h>
#include <Json.h>
#include <RailCamera/RailCamera.h>
#include <LoadScene/LoadingManager.h>

using namespace Engine;

/// <summary>
/// OverScene.h
/// ゲームオーバーシーンのヘッダーファイル
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
	// UI点滅
	void Blinking();
private:
	// カメラ
	std::unique_ptr<RailCamera> railCamera_ = nullptr;
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

	// load
	std::unique_ptr<LoadingManager> loadingManager_ = nullptr;
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
	bool isLoad_;

	// デバッグ用
	Vector2 pos = {};
	Vector2 scale = {};
};
