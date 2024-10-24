#include "BossBullet.h"
#include <Player/Player.h>

/// <summary>
/// BossBullet.cpp
/// ボス弾生成のソースファイル
/// </summary>

void BossBullet::Initialize(Vector3 pos, Vector3 velocity)
{
	transform = { {1.0f,1.0f,1.5f},{0.0f,0.0f,0.0f},{pos.x,pos.y,pos.z} };

	model_ = std::make_unique<Model>();
	model_->Initialize("cube.obj", transform);

	velocity_ = velocity;

	worldtransform_.scale = transform.scale;
	worldtransform_.rotate = transform.rotate;
	worldtransform_.translate = transform.translate;

	// z方向に伸びた形状
	worldtransform_.scale.x = 0.5f;
	worldtransform_.scale.y = 0.5f;
	worldtransform_.scale.z = 3.0f;

	// Y軸周り角度（Θy）
	worldtransform_.rotate.y = std::atan2(velocity_.x, velocity_.z);

	float velocityXZ = sqrt((velocity_.x * velocity_.x) + (velocity_.z * velocity_.z));

	worldtransform_.rotate.x = std::atan2(-velocity_.y, velocityXZ);

	worldtransform_.UpdateMatrix();
}

void BossBullet::Update()
{
	// 敵弾から自キャラへのベクトル計算
	Vector3 toPlayer;
	toPlayer.x = player_->GetPos().x - transform.translate.x;
	toPlayer.y = player_->GetPos().y - transform.translate.y;
	toPlayer.z = player_->GetPos().z - transform.translate.z;

	float t = 0.0f;

	// 引数で受け取った速度をメンバ変数に代入
	velocity_ = Slerp(toPlayer, transform.translate, t);

	velocity_.x *= 0.5f;
	velocity_.y *= 0.5f;
	velocity_.z *= 0.5f;

	// Y軸周り角度（Θy）
	worldtransform_.rotate.y = std::atan2(velocity_.x, velocity_.z);

	float velocityXZ = sqrt((velocity_.x * velocity_.x) + (velocity_.z * velocity_.z));
	worldtransform_.rotate.x = std::atan2(-velocity_.y, velocityXZ);

	// 座標を移動させる(1フレーム分の移動量を足しこむ)
	worldtransform_.translate.x += velocity_.x;
	worldtransform_.translate.y += velocity_.y;
	worldtransform_.translate.z += velocity_.z;

	worldtransform_.UpdateMatrix();
	model_->SetWorldTransform(worldtransform_);

	// 時間経過でデス
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}
}

void BossBullet::Draw(Camera* camera, uint32_t index)
{
	model_->Draw(camera, index);
}
