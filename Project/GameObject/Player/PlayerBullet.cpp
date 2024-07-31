#include "PlayerBullet.h"

void PlayerBullet::Initialize(Vector3 pos, Vector3 velocity)
{
	transform = { {0.3f,0.3f,0.5f},{0.0f,0.0f,0.0f},{pos.x,pos.y,pos.z} };

	model_ = std::make_unique<Model>();
	model_->Initialize("cube.obj", transform);

	worldtransform_.scale = transform.scale;
	worldtransform_.rotate = transform.rotate;
	worldtransform_.translate = transform.translate;
	worldtransform_.UpdateMatrix();

	velo = velocity;
}

void PlayerBullet::Update()
{
	worldtransform_.translate.x += velo.x;
	worldtransform_.translate.y += velo.y;
	worldtransform_.translate.z += velo.z;
	worldtransform_.UpdateMatrix();
	model_->SetWorldTransform(worldtransform_);

	// 時間経過でデス
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}
}

void PlayerBullet::Draw(Camera* camera, uint32_t index)
{
	if (isDead_ == false) {
		model_->Draw(camera, index);
	}
}
