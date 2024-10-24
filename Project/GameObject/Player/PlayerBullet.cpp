#include "PlayerBullet.h"

/// <summary>
/// PlayerBullet.cpp
/// プレイヤー弾生成のソースファイル
/// </summary>

void PlayerBullet::Initialize(Vector3 pos, Vector3 velocity)
{
	transform = { {0.05f,0.05f,0.05f},{0.0f,0.0f,0.0f},{pos.x,pos.y,pos.z - 5.0f} };

	model_ = std::make_unique<Model>();
	model_->Initialize("misairu.obj", transform);

	worldtransform_.scale = transform.scale;
	worldtransform_.rotate = transform.rotate;
	worldtransform_.translate = transform.translate;
	worldtransform_.UpdateMatrix();

	velo = velocity;

	// Y軸周り角度（Θy）
	worldtransform_.rotate.y = std::atan2(velo.x, velo.z);

	float velocityXZ = sqrt((velo.x * velo.x) + (velo.z * velo.z));
	worldtransform_.rotate.x = std::atan2(-velo.y, velocityXZ);
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
