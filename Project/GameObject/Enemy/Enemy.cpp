#include "Enemy.h"
#include "Player/Player.h"
#include "GameScene.h"

void Enemy::Initialize(Vector3 pos)
{
	transform_ = { {0.5f,0.5f,0.5f},{0.0f,0.0f,0.0f},{pos.x,pos.y,pos.z} };

	model_ = std::make_unique<Model>();
	model_->Initialize("cube.obj", transform_);

	worldtransform_.scale = transform_.scale;
	worldtransform_.rotate = transform_.rotate;
	worldtransform_.translate = transform_.translate;
	worldtransform_.UpdateMatrix();

	isDead_ = false;

	enemyTex = textureManager_->Load("resources/white.png");

	attackTimer = 10;
}

void Enemy::Update()
{
	worldtransform_.UpdateMatrix();
	float kMaxAttack = 600.0f;

	attackTimer--;

	if (attackTimer <= 0) {

		if (isDead_ == false) {
			rensya--;

			// 攻撃処理
			if (rensya < 0) {

				if (worldtransform_.translate.z - player_->GetPos().z <= kMaxAttack) {
					//Attack();
					rensyanum += rensyaNumSpeed;
				}

				if (rensyanum < kMaxRensyaNum) {
					rensya = 10;
				}
			}
		}

		// 発射タイマーを初期化
		if (rensyanum >= kMaxRensyaNum) {
			attackTimer = kFireInterval;
			rensyanum = 0;
		}
	}

	// 近づいたら動き出す
	if (worldtransform_.translate.z - player_->GetPos().z <= kMaxAttack) {

		const float kMoveSpeed = 0.005f;

		if (worldtransform_.translate.y < posParam.y) {
			speedY += kMoveSpeed;
		}
		else if (worldtransform_.translate.y >= posParam.y) {
			speedY -= kMoveSpeed;
		}

		worldtransform_.translate.y += speedY;
	}

	model_->SetWorldTransform(worldtransform_);

	Vector3 end = player_->GetPos();
	Vector3 start = worldtransform_.translate;

	Vector3 diff;
	diff.x = end.x - start.x;
	diff.y = end.y - start.y;
	diff.z = end.z - start.z;

	diff = Normalize(diff);

	Vector3 velocity_(diff.x, diff.y, diff.z);

	// Y軸周り角度（Θy）
	worldtransform_.rotate.y = std::atan2(velocity_.x, velocity_.z);
	float velocityXZ = sqrt((velocity_.x * velocity_.x) + (velocity_.z * velocity_.z));
	worldtransform_.rotate.x = std::atan2(-velocity_.y, velocityXZ);
	worldtransform_.UpdateMatrix();

	if (ImGui::TreeNode("Enemy")) {
		ImGui::DragFloat3("Rotate.y ", &worldtransform_.rotate.x, 0.01f);
		ImGui::DragFloat3("Transform", &worldtransform_.translate.x, 0.01f);
		ImGui::Checkbox("isDead", &isDead_);
		ImGui::DragInt("rensyanum : &d", &rensyanum);
		ImGui::DragInt("rensya : &d", &rensya);
		ImGui::TreePop();
	}
}

void Enemy::Draw(Camera* camera)
{
	if (isDead_ == false) {
		model_->Draw(camera, enemyTex);
	}
	else {
		
	}
}

void Enemy::Attack()
{
	if (player_ == nullptr || gameScene_ == nullptr) return;

	Vector3 end = player_->GetPos();
	Vector3 start = worldtransform_.translate;

	Vector3 diff;
	diff.x = end.x - start.x;
	diff.y = end.y - start.y;
	diff.z = end.z - start.z;

	diff = Normalize(diff);

	Vector3 velocity_(diff.x, diff.y, diff.z);


	// 弾を生成し、初期化
	std::unique_ptr<EnemyBullet> newBullet = std::make_unique<EnemyBullet>();
	newBullet->SetPlayer(player_);
	newBullet->Initialize(worldtransform_.translate, velocity_);
	// 弾を登録
	gameScene_->AddEnemyBullet(std::move(newBullet));
}

void Enemy::OnCollision()
{
	isDead_ = true;
}

Vector3 Enemy::GetWorldPosition() const
{
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldtransform_.matWorld.m[3][0];
	worldPos.y = worldtransform_.matWorld.m[3][1];
	worldPos.z = worldtransform_.matWorld.m[3][2];

	return worldPos;
}
