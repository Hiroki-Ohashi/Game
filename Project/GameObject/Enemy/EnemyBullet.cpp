#include "EnemyBullet.h"
#include "Player.h"

void EnemyBullet::Init(Vector3 pos, Vector3 velocity)
{
	transform = { { 0.05f,0.05f,0.2f},{0.0f,0.0f,0.0f},{pos.x,pos.y,pos.z} };

	model_ = std::make_unique<Model>();
	model_->Initialize("cube.obj", transform);

	velocity_ = velocity;
}

void EnemyBullet::Update()
{
	// 敵弾から自キャラへのベクトル計算
	Vector3 toPlayer;
	toPlayer.x = player_->GetPosition().x - transform.translate.x;
	toPlayer.y = player_->GetPosition().y - transform.translate.y;
	toPlayer.z = player_->GetPosition().z - transform.translate.z;

	float t = 0.1f;

	// 引数で受け取った速度をメンバ変数に代入
	velocity_ = SLerp(toPlayer,transform.translate,t);

	velocity_.x *= 0.1f;
	velocity_.y *= 0.1f;
	velocity_.z *= 0.1f;

	// Y軸周り角度（Θy）
	model_->worldTransform_.rotate.y = std::atan2(velocity_.x, velocity_.z);

	float velocityXZ = sqrt((velocity_.x * velocity_.x) + (velocity_.z * velocity_.z));
	model_->worldTransform_.rotate.x = std::atan2(-velocity_.y, velocityXZ);

	// 座標を移動させる(1フレーム分の移動量を足しこむ)
	transform.translate.x += velocity_.x;
	transform.translate.y += velocity_.y;
	transform.translate.z += velocity_.z;

	model_->worldTransform_.translate = transform.translate;

	// 時間経過でデス
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}
}

void EnemyBullet::Draw(Camera* camera, uint32_t index)
{
	model_->Draw(camera, index);
}

void EnemyBullet::SetPlayer(Player* player) { player_ = player; }
