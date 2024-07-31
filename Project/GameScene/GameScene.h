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

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")

class GameScene : public IScene {
public:
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
  
	void CheckAllCollisions();

	/// <summary>
	/// 敵発生データの読み込み
	/// </summary>
	void LoadEnemyPopData();

	/// <summary>
	/// 敵発生コマンドの更新
	/// </summary>
	void UpdateEnemyPopCommands();

	void EnemySpown(Vector3 pos);
	void AddEnemyBullet(EnemyBullet* enemyBullet);
	void AddBossBullet(BossBullet* bossBullet);
	void AddEnemy(Enemy* enemy);

private:
	Camera camera_;
	TextureManager* textureManager_ = TextureManager::GetInstance();
	std::unique_ptr<PostProcess> postProcess_ = nullptr;

	// プレイヤー
	std::unique_ptr<Player> player_;

	// 敵
	std::list<Enemy*> enemys_;
	std::unique_ptr<Boss> boss_;
	// 敵弾
	std::list<EnemyBullet*> enemyBullets_;
	std::list<BossBullet*> bossBullets_;
  
	std::unique_ptr<Json> json_ = nullptr;
	LevelData* levelData_ = nullptr;
  
	// ステージ
	std::unique_ptr<Stage> stage_;
	// 天球
	std::unique_ptr<Skydome> skydome_;
private:
	// テクスチャ
	uint32_t enemyBulletTex;
	uint32_t bossBulletTex;
	uint32_t uv;

	Vector3 pos_ = { 0.0f, 0.0f, 500.0f };
	Vector3 pos2_ = { 0.0f, 80.0f, 700.0f };

	// 待機タイマー
	int32_t waitTimer_;
	// 待機フラグ
	bool isWait_;

	// 敵発生コマンド
	std::stringstream enemyPopCommands;
};
