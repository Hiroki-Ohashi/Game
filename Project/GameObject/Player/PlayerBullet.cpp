#include "PlayerBullet.h"

void PlayerBullet::Initialize(Vector3 pos)
{
	transform = { {0.3f,0.3f,0.5f},{0.0f,0.0f,0.0f},{pos.x,pos.y,pos.z} };

	model_ = std::make_unique<Model>();
	model_->Initialize("cube.obj", transform);

	worldtransform_.scale = transform.scale;
	worldtransform_.rotate = transform.rotate;
	worldtransform_.translate = transform.translate;
	worldtransform_.UpdateMatrix();
}

void PlayerBullet::Update()
{
	transform.translate.z += 2.0f;

	worldtransform_.translate = transform.translate;
	model_->SetWorldTransform(worldtransform_);

	// 時間経過でデス
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}
}

void PlayerBullet::Draw(Camera* camera, uint32_t index)
{
	model_->Draw(camera, index);
}
