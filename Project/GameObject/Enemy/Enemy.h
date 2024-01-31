#pragma once
#include "Model.h"
#include "EnemyBullet.h"
#include "TextureManager.h"

class Player;

class Enemy {
public:
	~Enemy();

	void Init();
	void Update();
	void Draw(uint32_t index, Camera* camera, uint32_t index2);

	void Attack();

	void ApproachUpdate();

	void LeaveUpdate();

	void SetPlayer(Player* player);

	void OnCollision() { isDead_ = true; }

	Vector3 GetPosition() { return transform.translate; }
	// 弾リストを取得
	const std::list<EnemyBullet*>& GetBullets() const { return bullets_; }

	// 行動フェーズ
	enum class Phase {
		Approach, // 接近する
		Leave,    // 離脱する
	};

private:
	Model* model_ = nullptr;
	TextureManager* textureManager_ = TextureManager::GetInstance();
	Transform transform;

	std::list<EnemyBullet*> bullets_;

	// 発射タイマー
	int32_t attackTimer = 10;
	static const int kFireInterval = 60;

	int bulletTex;

	// フェーズ
	Phase phase_ = Phase::Approach;
	// メンバ関数ポインタのテーブル
	static void (Enemy::* phasePFuncTable[])();

	Player* player_ = nullptr;

	bool isDead_;
};