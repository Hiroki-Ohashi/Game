#pragma once
#include <TextureManager.h>
#include <Model.h>
#include <Input.h>
#include "PlayerBullet.h"

class Player {
public:
	~Player();

	void Initialize();
	void Update();
	void Draw(Camera* camera_);
	void BulletDraw(Camera* camera_);

	void OnCollision() { isHit_ = true; }

	Vector3 GetPos() { return worldtransform_.translate; }
	Vector3 GetReticlePos() { return reticleWorldtransform_.translate; }
	Vector3 Get3DWorldPosition();
	Vector3 GetVelocity() { return velocity_; }
	// 弾リストを取得
	 std::vector<std::unique_ptr<PlayerBullet>>& GetBullets() { return bullets_; }
private:
	void Attack();
private:
	TextureManager* textureManager_ = TextureManager::GetInstance();
	Input* input_ = Input::GetInsTance();

	WorldTransform worldtransform_;
	EulerTransform transform_;
	std::unique_ptr<Model> model_;

	std::unique_ptr<Model> reticleModel_;
	WorldTransform reticleWorldtransform_;
	EulerTransform reticleTransform_;

	std::vector<std::unique_ptr<PlayerBullet>> bullets_;

	uint32_t bulletTex;
	uint32_t playerTex;
	uint32_t reticleTex;
	uint32_t hit;

	Vector3 velocity_;

	int32_t hitTimer_;
	bool isHit_;

	bool isLeft;
	bool isRight;
};