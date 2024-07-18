#include "Enemy.h"
#include "Player/Player.h"
#include "GameScene.h"

void Enemy::Initialize(Vector3 pos)
{
	transform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{pos.x,pos.y,pos.z} };

	model_ = std::make_unique<Model>();
	model_->Initialize("cube.obj", transform_);

	worldtransform_.scale = transform_.scale;
	worldtransform_.rotate = transform_.rotate;
	worldtransform_.translate = transform_.translate;
	worldtransform_.UpdateMatrix();

	isDead_ = false;

	enemyTex = textureManager_->Load("resources/black.png");
}

void Enemy::Update()
{

	attackTimer--;

	if (attackTimer <= 0) {

		if (isDead_ == false) {
			// 攻撃処理
			Attack();
		}

		// 発射タイマーを初期化
		attackTimer = kFireInterval;
	}

	model_->SetWorldTransform(worldtransform_);

	if (ImGui::TreeNode("Enemy")) {
		ImGui::DragFloat3("Rotate.y ", &worldtransform_.rotate.x, 0.01f);
		ImGui::DragFloat3("Transform", &worldtransform_.translate.x, 0.01f);
		ImGui::Checkbox("isDead", &isDead_);
		ImGui::TreePop();
	}
}

void Enemy::Draw(Camera* camera)
{
	if (isDead_ == false) {
		model_->Draw(camera, enemyTex);
	}
}

void Enemy::Attack()
{
	assert(player_);

	Vector3 end = player_->GetPos();
	Vector3 start = transform_.translate;

	Vector3 diff;
	diff.x = end.x - start.x;
	diff.y = end.y - start.y;
	diff.z = end.z - start.z;

	diff = Normalize(diff);

	Vector3 velocity(diff.x, diff.y, diff.z);


	// 弾を生成し、初期化
	EnemyBullet* newBullet = new EnemyBullet();
	newBullet->SetPlayer(player_);
	newBullet->Initialize(transform_.translate, velocity);
	// 弾を登録
	gameScene_->AddEnemyBullet(newBullet);
}
