#pragma once
#include <Camera.h>
#include <TextureManager.h>
#include <WorldTransform.h>
#include <Model.h>
#include "EnemyBullet.h"
#include "Collider.h"
#include "Particle.h"
using namespace Engine;

class Player;
class GameScene;

/// <summary>
/// Enemy.h
/// 敵生成のヘッダーファイル
/// </summary>

// Enemyクラス
class Enemy : public Collider {
public:
	// 初期化処理
	void Initialize(Vector3 pos);
	// 更新処理
	void Update();
	// 描画処理
	void Draw(Camera* camera);
	// 攻撃処理
	void Attack();
	// 当たり判定処理
	void OnCollision() override;
	Vector3 GetWorldPosition() const override;

	// 死亡判定
	void SetIsDead(bool isDead) { isDead_ = isDead; }
	bool IsDead(){ return isDead_; }

	// Getter
	Vector3 GetPos() { return worldtransform_.translate; }

	// Setter
	void SetPosition(Vector3 pos) { 
		worldtransform_.translate = pos;
		posParam = pos;
	}
	void SetRotation(Vector3 rotate) { worldtransform_.rotate = rotate; }
	void SetScale(Vector3 scale) { worldtransform_.scale = scale; }
	void SetLight(bool isLight) { model_->SetLight(isLight); }
	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }
	void SetPlayer(Player* player) { player_ = player; }
private:
	// シングルトン呼び出し
	TextureManager* textureManager_ = TextureManager::GetInstance();
	WorldTransform worldtransform_;
	EulerTransform transform_;
	std::unique_ptr<Model> model_;

	std::unique_ptr<Engine::Particles> particle_ = nullptr;

	// 呼び出し
	Player* player_ = nullptr;
	GameScene* gameScene_ = nullptr;
private:
	// テクスチャ
	uint32_t enemyTex;
	// 発射タイマー
	int32_t attackTimer = 10;
	static const int kFireInterval = 120;
	bool isDead_;
	int rensya = 10;
	int rensyanum = 0;

	// 初期位置
	Vector3 posParam;

	float speedY = 0.3f;
};