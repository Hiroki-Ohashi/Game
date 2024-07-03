#pragma once
#include <Camera.h>
#include <TextureManager.h>
#include <WorldTransform.h>
#include <Model.h>
#include "EnemyBullet.h"

class Player;
class GameScene;

class Enemy {
public:
	void Initialize(Vector3 pos);
	void Update();
	void Draw(Camera* camera);

	void Attack();

	Vector3 GetPos() { return worldtransform_.translate; }
	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }
	void SetPlayer(Player* player) { player_ = player; }
private:
	TextureManager* textureManager_ = TextureManager::GetInstance();
	WorldTransform worldtransform_;
	EulerTransform transform_;
	std::unique_ptr<Model> model_;
	std::list<EnemyBullet*> bullets_;

	Player* player_ = nullptr;
	GameScene* gameScene_ = nullptr;
private:
	uint32_t enemyTex;
	// 発射タイマー
	int32_t attackTimer = 10;
	static const int kFireInterval = 120;
	bool isDead_ = false;
};