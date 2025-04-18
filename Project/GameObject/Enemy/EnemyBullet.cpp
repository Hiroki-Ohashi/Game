#include "EnemyBullet.h"
#include <Player/Player.h>
#include <cmath>

/// <summary>
/// EnemyBullet.cpp
/// 弾生成のソースファイル
/// </summary>

void EnemyBullet::Initialize(Vector3 pos, Vector3 velocity)
{
	transform = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{pos.x,pos.y,pos.z} };

	model_ = std::make_unique<Model>();
	model_->Initialize("misairu.obj", transform);

	worldtransform_.scale = transform.scale;
	worldtransform_.rotate = transform.rotate;
	worldtransform_.translate = transform.translate;
	worldtransform_.UpdateMatrix();

	velocity_ = velocity;

	// Y軸周り角度（Θy）
	worldtransform_.rotate.y = std::atan2(velocity_.x, velocity_.z);
	float velocityXZ = sqrt((velocity_.x * velocity_.x) + (velocity_.z * velocity_.z));
	worldtransform_.rotate.x = std::atan2(-velocity_.y, velocityXZ);
	worldtransform_.UpdateMatrix();

	isDead_ = false;

	// 衝突属性を設定
	SetCollosionAttribute(kcollisionAttributeEnemy);
	// 衝突対象を自分の属性以外に設定
	SetCollisionMask(0xffffffff);

	emitter.count = 10;
	emitter.frequency = 0.016f;
	emitter.frequencyTime = 0.0f;
	emitter.transform.translate = worldtransform_.translate;
	emitter.transform.rotate = worldtransform_.rotate;
	emitter.transform.scale = { 1.0f, 1.0f, 1.0f };

	particle_ = std::make_unique<Particles>();
	particle_->Initialize("board.obj", worldtransform_.translate, emitter);
	particle_->SetEmitter(emitter);
}

void EnemyBullet::Reset(Vector3 pos, Vector3 velocity)
{
	Initialize(pos, velocity); // 弾を再初期化
	isDead_ = false;
}

void EnemyBullet::Update()
{
	// 敵弾から自キャラへのベクトル計算
	Vector3 toPlayer;
	toPlayer.x = player_->GetPos().x - transform.translate.x;
	toPlayer.y = player_->GetPos().y - transform.translate.y;
	toPlayer.z = player_->GetPos().z - transform.translate.z;

	float t = 0.8f;

	// 引数で受け取った速度をメンバ変数に代入
	velocity_ = Slerp(toPlayer, transform.translate, t);

	velocity_.x *= 2.5f;
	velocity_.y *= 2.5f;
	velocity_.z *= 2.5f;

	//// Y軸周り角度（Θy）
	//worldtransform_.rotate.y = std::atan2(velocity_.x, velocity_.z);

	//float velocityXZ = sqrt((velocity_.x * velocity_.x) + (velocity_.z * velocity_.z));
	//worldtransform_.rotate.x = std::atan2(-velocity_.y, velocityXZ);

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

	// エミッタの位置と回転を設定
	emitter.transform.translate = { worldtransform_.translate.x, worldtransform_.translate.y, worldtransform_.translate.z - 10.0f };
	particle_->SetEmitter(emitter);

	// パーティクルを生成して更新
	particle_->Update();

	/*if (ImGui::TreeNode("EnemyBullet")) {
		ImGui::DragFloat3("Rotate.y ", &worldtransform_.rotate.x, 0.01f);
		ImGui::DragFloat3("Transform", &worldtransform_.translate.x, 0.01f);
		ImGui::Checkbox("isDead", &isDead_);
		ImGui::TreePop();
	}*/
}

void EnemyBullet::Draw(Camera* camera, uint32_t index, uint32_t index2)
{
	if (isDead_ == false) {
		model_->Draw(camera, index);
		particle_->Draw(camera, index2);
	}
}

void EnemyBullet::OnCollision()
{
	isDead_ = true;
}

Vector3 EnemyBullet::GetWorldPosition() const
{
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldtransform_.matWorld.m[3][0];
	worldPos.y = worldtransform_.matWorld.m[3][1];
	worldPos.z = worldtransform_.matWorld.m[3][2];

	return worldPos;
}
