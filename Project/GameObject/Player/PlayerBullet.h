#pragma once
#include <Camera.h>
#include <Model.h>

class PlayerBullet {
public:
	void Initialize(Vector3 pos);
	void Update();
	void Draw(Camera* camera, uint32_t index);

	void OnCollision() { isDead_ = true; }

	bool IsDead() const { return isDead_; }
	Vector3 GetPos() { return worldtransform_.translate; }
private:

	std::unique_ptr<Model> model_;
	EulerTransform transform;
	WorldTransform worldtransform_;
	// デスタイマー
	static const int32_t kLifeTime = 60 * 2;
	int32_t deathTimer_ = kLifeTime;
	bool isDead_ = false;

private:
};