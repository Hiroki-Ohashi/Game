#pragma once
#include <Camera.h>
#include <TextureManager.h>
#include <WorldTransform.h>
#include <Model.h>
#include "EnemyBullet.h"
#include "Collider.h"
#include "Particle.h"
#include "EnemyBulletPool.h"
#include "Sprite.h"
#include "CollisionConfig.h"
#include "BaseEnemyState.h"

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
	void Update(EnemyType type, Camera* camera_);
	void FixedUpdate(Camera* camera_);
	void FryUpdate(Camera* camera_);
	// 描画処理
	void Draw(Camera* camera);
	void DrawUI();
	void DrawParticle(Camera* camera);
	// 攻撃処理
	void Attack();
	// 当たり判定処理
	void OnCollision() override;

	void ChangeState(BaseEnemyState* newState);

	// 死亡判定
	void SetIsDead(bool isDead) { isDead_ = isDead; }
	bool IsDead(){ return isDead_; }
	bool IsActive() const { return !isDead_;}

	// Getter
	Vector3 GetPos() const { return position_; }
	Vector3 GetPrePos() const { return previousPosition_; }
	Vector3 GetScreenPos() { return positionReticle; }
	Vector3 GetWorldPosition() const override;
	bool GetIsLockOn() const { return isLockOn_; }
	bool GetIsPossibillityLock() const { return isPossibillityLock; }

	void UpdatePosition(Vector3 newPosition) {
		previousPosition_ = position_;
		position_ = newPosition;
	}

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
	void SetisLockOn(bool isLockOn) { isLockOn_ = isLockOn; }
	void SetEnemySpeed(Vector3 speed_) { enemySpeed = speed_; }
private:
	// シングルトン呼び出し
	TextureManager* textureManager_ = TextureManager::GetInstance();

	WorldTransform worldtransform_;
	EulerTransform transform_;
	std::unique_ptr<Model> model_;

	// 呼び出し
	Player* player_ = nullptr;
	GameScene* gameScene_ = nullptr;
	EnemyBulletPool bulletPool_;

	std::unique_ptr<Sprite> enemySprite_;
	std::unique_ptr<Particles> particle_ = nullptr;

private:
	// テクスチャ
	uint32_t enemyTex;
	uint32_t enemyBulletTex;
	uint32_t lockOnTex;
	uint32_t lockTex;
	uint32_t bakuhatuTex;
	
	// 発射タイマー
	int32_t attackTimer = 10;
	static const int kFireInterval = 60;
	
	int rensya = 10;
	int rensyanum = 0;
	const int rensyaNumSpeed = 1;
	const int kMaxRensyaNum = 3;

	Vector3 positionReticle;

	// 初期位置
	Vector3 posParam = {0.0f, 80.0f, 0.0f};

	float speedY = 0.3f;
	float kRotSpeed = 1.0f;
	float kScaleSpeed = 0.05f;

	bool isDead_;
	float kMaxAttack = 2500.0f;

	bool isLockOn_;
	bool isPossibillityLock;

	Vector3 position_;
	Vector3 previousPosition_;

	Vector3 enemySpeed = { 0.0f, 0.0f, 20.0f };

	// ステート
	BaseEnemyState* state;

	// particleEmitter
	Emitter emitter{};
};