#pragma once
#include "Model.h"
#include "EnemyBullet.h"
#include "TextureManager.h"

class Player;
class GameScene;

class Enemy {
public:
	~Enemy();

	void Init(Vector3 translation);
	void Update();
	void Draw(uint32_t index, Camera* camera);

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

	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

private:
	std::unique_ptr<Model> model_ = nullptr;
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
	GameScene* gameScene_ = nullptr;

	bool isDead_;
};