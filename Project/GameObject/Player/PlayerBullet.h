#pragma once
#include "Model.h"
#include "WorldTransform.h"

class PlayerBullet {
public:
	~PlayerBullet();

	void Init(Vector3 pos);
	void Update();
	void Draw(Camera* camera, uint32_t index);

	bool IsDead() const { return isDead_; }
	Vector3 GetPosition() { return transform.translate; }
	void OnCollision() { isDead_ = true; }

private:
	std::unique_ptr<Model> model_;
	Transform transform;
	TextureManager* textureManager_ = TextureManager::GetInstance();

	static const int32_t kLifeTime = 60 * 5;
	// デスタイマー
	int32_t deathTimer_ = kLifeTime;

	bool isDead_ = false;
};