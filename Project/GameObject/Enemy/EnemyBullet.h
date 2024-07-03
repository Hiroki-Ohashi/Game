#pragma once
#include <WorldTransform.h>
#include <Model.h>

class Player;

class EnemyBullet {
public:
	void Initialize(Vector3 pos, Vector3 velocity);
	void Update();
	void Draw(Camera* camera, uint32_t index);

	bool IsDead() const { return isDead_; }
	Vector3 GetPosition() { return transform.translate; }
	void SetPlayer(Player* player) { player_ = player; }
private:

	std::unique_ptr<Model> model_;
	EulerTransform transform;
	WorldTransform worldtransform_;
	// 自機
	Player* player_ = nullptr;
	// 速度
	Vector3 velocity_;

	// デスタイマー
	static const int32_t kLifeTime = 60 * 5;
	int32_t deathTimer_ = kLifeTime;
	bool isDead_ = false;
};