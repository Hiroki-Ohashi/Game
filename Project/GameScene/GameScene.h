#pragma once
#define NOMINMAX
#include <windows.h>
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
#include <Player/Player.h>
#include <Stage/Stage.h>
#include <SkyBox/Skydome.h>
#include <RailCamera/RailCamera.h>
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

	// 当たり判定
	void CheckAllCollisions();
	// リスト登録
	void AddEnemyBullet(std::unique_ptr<EnemyBullet> enemyBullet);

private:
	// シェイクを開始する関数
	void ShakeCamera();

	// ロックオン処理
	void LockOnEnemy();
	void Pose(XINPUT_STATE joyState_);

	// スタート演出
	void Start();
	// クリア演出
	void Clear();
	// ゲームオーバー演出
	void Over();

private:
	// カメラ
	std::unique_ptr<RailCamera> railCamera_ = nullptr;
	// シングルトン呼び出し
	TextureManager* textureManager_ = TextureManager::GetInstance();

	// postProcess
	std::unique_ptr<PostProcess> postProcess_ = nullptr;

	// プレイヤー
	std::unique_ptr<Player> player_;

	// 敵弾
	std::vector<std::unique_ptr<EnemyBullet>> enemyBullets_;

	// スタート演出Model
	std::unique_ptr<Sprite> ready_ = nullptr;
	std::unique_ptr<Model> go_ = nullptr;

	// json
	std::unique_ptr<Json> json_ = nullptr;
	LevelData* levelData_ = nullptr;

	// Stage
	std::unique_ptr<Stage> stage_ = nullptr;
  
	// 天球
	std::unique_ptr<Skydome> skydome_;

	std::vector<std::unique_ptr<Particles>> particlesList;

	// pose
	std::unique_ptr<Sprite> sentaku_ = nullptr;

	// 各補助関数
	void CheckSphereCollisions(std::list<Collider*>& colliders);
	void CheckAABBCollisionsWithObjects();
	void CheckReticleLockOn();
	void CheckOBBCollisionsWithPlayer();

	// 当たり判定
	void CheckCollisionPair(Collider* colliderA, Collider* colliderB);
	void CheckAABBCollisionPair(Collider* colliderA, Collider* colliderB);
	bool IsOBBColliding(const OBB& a, const OBB& b);
private:
	// テクスチャ
	uint32_t enemyBulletTex;
	uint32_t bossBulletTex;
	uint32_t uv;
	uint32_t ready;
	uint32_t go;
	uint32_t backTitle;
	uint32_t retry;
	uint32_t kemuri;

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
	const int kMaxTime = 180;
	const int timerSpeed = 1;

	// ブラー
	float blurStrength_ = 0.3f;
	const float kDefaultBlurStrength_ = 0.01f;
	float minusBlurStrength_ = 0.005f;

	// ノイズ
	float noiseStrength;
	bool isNoise;
	const float kdamageNoise = 3.0f;
	const float kMaxNoiseStrength = 100.0f;
	const float plusNoiseStrength = 1.0f;

	// ゴールライン
	float goalline = 98000.0f;
	bool isGoal_;

	// デバッグ用仮変数;
	Vector2 pos = {};
	Vector3 pos_ = {};
	Vector2 scale = {};
	EulerTransform transform_;

	// イージング変数
	float start = 0.0f;
	float end = 6.29f;
	float frame;
	float endFrame = 100.0f;

	bool isPose_;
	bool prevBackButtonState_ = false;

	// scene
	uint32_t scenePrev;

	// collision
	const float reticleHalfWidth = 50.0f;
	const float enemyHalfWidth = 25.0f;
};
