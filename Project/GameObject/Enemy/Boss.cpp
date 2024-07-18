#include "Boss.h"
#include "Player/Player.h"
#include "GameScene.h"

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

	hp_ = 10;

	enemyTex = textureManager_->Load("resources/black.png");
}

void Boss::Update()
{

	transform_.translate.x += speedX;
	transform_.translate.y += speedY;

	if (transform_.translate.x >= 30.0f) {
		speedX *= -1;
	}
	else if (transform_.translate.x <= -30.0f) {
		speedX *= -1;
	}

	if (transform_.translate.y >= 30.0f) {
		speedY *= -1;
	}
	else if (transform_.translate.y <= -30.0f) {
		speedY *= -1;
	}

	transform_.translate.z += 0.5f;

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
		model_->Draw(camera, enemyTex);
	}
}

void Boss::Attack()
{
	Vector3 end = player_->GetPos();
	Vector3 start = transform_.translate;

	Vector3 diff;
	diff.x = end.x - start.x;
	diff.y = end.y - start.y;
	diff.z = end.z + start.z;

	diff = Normalize(diff);

	Vector3 velocity(diff.x, diff.y, diff.z);


	// 弾を生成し、初期化
	BossBullet* newBullet = new BossBullet();
	newBullet->SetPlayer(player_);
	newBullet->Initialize(transform_.translate, velocity);
	// 弾を登録
	gameScene_->AddBossBullet(newBullet);
}
