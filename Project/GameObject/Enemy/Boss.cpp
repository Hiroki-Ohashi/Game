#include "Boss.h"
#include "Player/Player.h"
#include "GameScene.h"

/// <summary>
/// Boss.cpp
/// ボス生成のソースファイル
/// </summary>

void Boss::Initialize(Vector3 pos)
{
	transform_ = { {15.0f,15.0f,15.0f},{0.0f,0.0f,0.0f},{pos.x,pos.y,pos.z} };

	model_ = std::make_unique<Model>();
	model_->Initialize("cube.obj", transform_);

	worldtransform_.scale = transform_.scale;
	worldtransform_.rotate = transform_.rotate;
	worldtransform_.translate = transform_.translate;
	worldtransform_.UpdateMatrix();

	isDead_ = false;
	isApproach = true;
	hp_ = 15;

	enemyTex = textureManager_->Load("resources/black.png");
	hit = textureManager_->Load("resources/red.png");

	isHit_ = false;
}

void Boss::Update()
{
	transform_.translate.z += 1.5f;

	if (isApproach == true) {
		transform_.translate.y -= downSpeedY;
		worldtransform_.rotate.y -= rotSpeedY;

		if (transform_.translate.y <= 25.0f) {
			downSpeedY = 0.0f;
			rotSpeedY = 0.0f;
			worldtransform_.rotate.y = 0.0f;
			isApproach = false;
		}
	}

	if (isApproach == false) {
		transform_.translate.x += speedX;
		transform_.translate.y += speedY;

		if (transform_.translate.x >= 30.0f) {
			speedX *= -1;
		}
		else if (transform_.translate.x <= -30.0f) {
			speedX *= -1;
		}

		if (transform_.translate.y >= 55.0f) {
			speedY *= -1;
		}
		else if (transform_.translate.y <= 0.0f) {
			speedY *= -1;
		}
	}

	attackTimer--;

	if (attackTimer <= 0) {

		if (isDead_ == false) {
			// 攻撃処理
			Attack();
		}

		// 発射タイマーを初期化
		attackTimer = kFireInterval;
	}

	if (hp_ <= 0) {
		isDead_ = true;
	}

	if (isHit_) {
		hitTimer_ += 1;
		if (hitTimer_ >= 5) {
			isHit_ = false;
			hitTimer_ = 0;
		}
	}

	worldtransform_.translate = transform_.translate;
	worldtransform_.UpdateMatrix();
	model_->SetWorldTransform(worldtransform_);

	if (ImGui::TreeNode("Boss")) {
		ImGui::DragFloat3("transform ", &worldtransform_.translate.x, 0.01f);
		ImGui::Text("HP:%d", hp_);
		ImGui::TreePop();
	}
}

void Boss::Draw(Camera* camera)
{
	if (isDead_ == false) {
		if (isHit_) {
			model_->Draw(camera, hit);
		}
		else {
			model_->Draw(camera, enemyTex);
		}
	}
}

void Boss::approach()
{
	worldtransform_.UpdateMatrix();
	model_->SetWorldTransform(worldtransform_);
}

void Boss::Attack()
{
	Vector3 end = player_->GetPos();
	Vector3 start = transform_.translate;

	Vector3 diff;
	diff.x = end.x - start.x;
	diff.y = end.y - start.y;
	diff.z = end.z - start.z;

	diff = Normalize(diff);

	Vector3 velocity(diff.x, diff.y, diff.z);


	// 弾を生成し、初期化
	std::unique_ptr<BossBullet> newBullet = std::make_unique<BossBullet>();
	newBullet->SetPlayer(player_);
	newBullet->Initialize(transform_.translate, velocity);
	// 弾を登録
	gameScene_->AddBossBullet(std::move(newBullet));
}

void Boss::OnCollision()
{
	hp_ -= 1;
	isHit_ = true;
}

Vector3 Boss::GetWorldPosition() const
{
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldtransform_.matWorld.m[3][0];
	worldPos.y = worldtransform_.matWorld.m[3][1];
	worldPos.z = worldtransform_.matWorld.m[3][2];

	return worldPos;
}
