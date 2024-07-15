#pragma once
#include <TextureManager.h>
#include <WorldTransform.h>
#include <Model.h>

class Boss {
public:
	void Initialize(Vector3 pos);
	void Update();
	void Draw(Camera* camera);

	void OnCollision() { hp_ -= 1; }
	void SetIsDead(bool isDead) { isDead_ = isDead; }

	bool IsDead() const { return isDead_; }
	Vector3 GetPos() { return worldtransform_.translate; }
private:
	TextureManager* textureManager_ = TextureManager::GetInstance();
	WorldTransform worldtransform_;
	EulerTransform transform_;
	std::unique_ptr<Model> model_;
	uint32_t hp_;
	uint32_t enemyTex;
	// 発射タイマー
	int32_t attackTimer = 10;
	static const int kFireInterval = 120;
	bool isDead_;
};