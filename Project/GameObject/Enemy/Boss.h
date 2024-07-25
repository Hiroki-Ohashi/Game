#pragma once
#include <Camera.h>
#include <TextureManager.h>
#include <WorldTransform.h>
#include <Model.h>
#include "BossBullet.h"

class Player;
class GameScene;

class Boss {
public:
	void Initialize(Vector3 pos);
	void Update();
	void Draw(Camera* camera);

	void approach();
	void Attack();

	void OnCollision() { hp_ -= 1; }
	void SetIsDead(bool isDead) { isDead_ = isDead; }

	bool IsDead() const { return isDead_; }
	Vector3 GetPos() { return worldtransform_.translate; }
	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }
	void SetPlayer(Player* player) { player_ = player; }
private:
	TextureManager* textureManager_ = TextureManager::GetInstance();
	WorldTransform worldtransform_;
	EulerTransform transform_;

	std::unique_ptr<Model> model_;
	std::list<BossBullet*> bullets_;

	Player* player_ = nullptr;
	GameScene* gameScene_ = nullptr;
private:
	uint32_t hp_;
	uint32_t enemyTex;

	float speedX = 0.5f;
	float speedY = 0.3f;

	float downSpeedY = 0.5f;
	float rotSpeedY = 0.3f;
	bool isApproach;
	// 発射タイマー
	int32_t attackTimer = 10;
	static const int kFireInterval = 120;
	bool isDead_;
};