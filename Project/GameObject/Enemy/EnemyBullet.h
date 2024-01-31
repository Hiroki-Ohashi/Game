#pragma once
#include "Model.h"
#include "WorldTransform.h"

class Player;

class EnemyBullet {
public:
	void Init(Vector3 pos, Vector3 velocity);
	void Update();
	void Draw(Camera* camera, uint32_t index);

	bool IsDead() const { return isDead_; }
	void OnCollision() { isDead_ = true; }

	void SetPlayer(Player* player);
	Vector3 GetPosition() { return transform.translate; }
private:

	std::unique_ptr<Model> model_;
	Transform transform;
	TextureManager* textureManager_ = TextureManager::GetInstance();

	WorldTransform worldTransform;

	static const int32_t kLifeTime = 60 * 5;
	// デスタイマー
	int32_t deathTimer_ = kLifeTime;

	bool isDead_ = false;

	// 速度
	Vector3 velocity_;

	// 自機
	Player* player_ = nullptr;
};
