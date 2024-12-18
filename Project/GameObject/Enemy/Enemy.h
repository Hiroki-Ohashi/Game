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

enum EnemyType{
	FRY,
	FIXEDENEMY
};

// Enemyクラス
class Enemy : public Collider {
public:
	// 初期化処理
	void Initialize(Vector3 pos, EnemyType type);
	// 更新処理
	void Update(EnemyType type);
	void FixedUpdate();
	void FryUpdate();
	// 描画処理
	void Draw(Camera* camera);
	// 攻撃処理
	void Attack();
	// 当たり判定処理
	void OnCollision() override;
	// 死亡アニメーション
	void DeadAnimation();

	// 死亡判定
	void SetIsDead(bool isDead) { isDead_ = isDead; }
	bool IsDead(){ return isDead_; }

	// Getter
	Vector3 GetPos() { return worldtransform_.translate; }
	Vector3 GetWorldPosition() const override;

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

	// 呼び出し
	Player* player_ = nullptr;
	GameScene* gameScene_ = nullptr;
private:
	// テクスチャ
	uint32_t enemyTex;
	// 発射タイマー
	int32_t attackTimer = 10;
	static const int kFireInterval = 120;
	
	int rensya = 10;
	int rensyanum = 0;
	const int rensyaNumSpeed = 1;
	const int kMaxRensyaNum = 3;

	// 初期位置
	Vector3 posParam;

	float speedY = 0.3f;
	float kRotSpeed = 1.0f;
	float kScaleSpeed = 0.05f;

	bool isDead_;
	bool isDeadAnimation_;
};