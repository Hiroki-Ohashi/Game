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
#include <PostProcess.h>
#include "Animation.h"
#include "Json.h"
#include "SkyBox.h"
#include <Player/Player.h>
#include <Skydome.h>
#include <Stage/Stage.h>
#include <Enemy/Enemy.h>
#include <Enemy/Boss.h>
using namespace Engine;

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")

/// <summary>
/// GameScene.h
/// ゲームシーンのヘッダーファイル
/// </summary>

// GameSceneクラス
class GameScene : public IScene {
public:
	// デストラクタ
	~GameScene();

	// 初期化
	void Initialize() override;
	// 読み込み
	void Update() override;
	// 描画
	void Draw() override;
	// ポストエフェクト
	void PostDraw() override;
  
	// 解放
	void Release();
	// 当たり判定
	void CheckAllCollisions();

	// リスト登録
	void AddEnemyBullet(std::unique_ptr<EnemyBullet> enemyBullet);
	void AddBossBullet(std::unique_ptr<BossBullet> bossBullet);

private:
	// カメラ
	Camera camera_;
	// シングルトン呼び出し
	TextureManager* textureManager_ = TextureManager::GetInstance();
	// postProcess
	std::unique_ptr<PostProcess> postProcess_ = nullptr;

	// プレイヤー
	std::unique_ptr<Player> player_;

	// 敵
	std::unique_ptr<Boss> boss_;
	// 敵弾
	std::vector<std::unique_ptr<EnemyBullet>> enemyBullets_;
	std::vector<std::unique_ptr<BossBullet>> bossBullets_;

	// スタート演出Model
	std::unique_ptr<Sprite> ready_ = nullptr;
	std::unique_ptr<Model> go_ = nullptr;

	// json
	std::unique_ptr<Json> json_ = nullptr;
	LevelData* levelData_ = nullptr;

	// UI
	std::unique_ptr<Sprite> ui_ = nullptr;

	// Stage
	std::unique_ptr<Stage> stage_ = nullptr;
  
	// 天球
	std::unique_ptr<Skydome> skydome_;

	std::unique_ptr<Particles> particle_ = nullptr;

	// 当たり判定
	void CheckCollisionPair(Collider* colliderA, Collider* colliderB);
	void CheckAABBCollisionPair(Collider* colliderA, Collider* colliderB);
private:
	// テクスチャ
	uint32_t enemyBulletTex;
	uint32_t bossBulletTex;
	uint32_t uv;
	uint32_t ready;
	uint32_t go;

	uint32_t hp5;
	uint32_t hp4;
	uint32_t hp3;
	uint32_t hp2;
	uint32_t hp1;
	uint32_t hp0;

	// bossParam
	Vector3 pos_ = { 0.0f, 25.0f, 500.0f };
	Vector3 pos2_ = { 0.0f, 100.0f, 4100.0f };

	// シェイク
	int randX = 0;
	int randY = 0;
	int32_t shakeTimer = 0;
	bool isShake = false;

	// 待機タイマー
	int32_t waitTimer_;
	// 待機フラグ
	bool isWait_;

	// シーン遷移
	bool isVignette_;
	bool isGameClear_;
	bool isGameOver_;

	// スタート演出
	bool isApploach_;
	int time_ = 0;
	int kMaxTime = 180;

	float blurStrength_ = 0.3f;
	const float kDefaultBlurStrength_ = 0.0f;
	float minusBlurStrength_ = 0.002f;

	float noiseStrength;
	const float kMaxNoiseStrength = 100.0f;
	const float plusNoiseStrength = 1.0f;

	EulerTransform transform_;

	// 敵発生コマンド
	std::stringstream enemyPopCommands;

	// ゴールライン
	float goalline = 3900.0f;

	// カメラoffset
	Vector3 cameraOffset = { 0.0f, 1.5f, 20.0f };
};
