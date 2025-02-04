#include "Enemy.h"
#include "Player/Player.h"
#include "GameScene.h"

void Enemy::Initialize(Vector3 pos, EnemyType type)
{
	transform_ = { {2.0f,2.0f,2.0f},{0.0f,0.0f,0.0f},{pos.x,pos.y,pos.z} };

	model_ = std::make_unique<Model>();
	if (type == FRY) {
		model_->Initialize("doron.obj", transform_);
	}
	else if (type == FIXEDENEMY) {
		model_->Initialize("fixed.obj", transform_);
	}

	worldtransform_.scale = transform_.scale;
	worldtransform_.rotate = transform_.rotate;
	worldtransform_.translate = transform_.translate;
	model_->SetWorldTransform(worldtransform_);
	worldtransform_.UpdateMatrix();

	lockTex = textureManager_->Load("resources/Lock.png");
	lockOnTex = textureManager_->Load("resources/reticle.png");

	enemySprite_ = std::make_unique<Sprite>();
	enemySprite_->Initialize({ 590.0f,310.0f }, { 50.0f,50.0f }, lockOnTex);
	enemySprite_->SetSize({ 50.0f,50.0f });
	enemySprite_->SetRotation({ 0.0f, 0.0f, -0.8f });

	/*particle_ = std::make_unique<Particles>();
	particle_->Initialize("board.obj", pos, 60);*/

	isDead_ = false;
	isLockOn_ = false;
	isPossibillityLock = false;

	enemyTex = textureManager_->Load("resources/white.png");
	enemyBulletTex = textureManager_->Load("resources/red.png");
	
	attackTimer = 10;

	// 衝突属性を設定
	SetCollosionAttribute(kcollisionAttributeEnemy);
	// 衝突対象を自分の属性以外に設定
	SetCollisionMask(kcollisionAttributePlayer);

	// 弾プールを初期化
	//bulletPool_.Initialize(200);
}

void Enemy::Update(EnemyType type, Camera* camera_)
{

	if (type == FRY) {
		FryUpdate(camera_);
	}
	else if (type == FIXEDENEMY) {
		FixedUpdate(camera_);
	}

	//particle_->SetPos(worldtransform_.translate);

	// 弾の更新
	//bulletPool_.Update();

	if (ImGui::TreeNode("Enemy")) {
		ImGui::DragFloat3("Rotate.y ", &worldtransform_.rotate.x, 0.01f);
		ImGui::DragFloat3("Transform", &worldtransform_.translate.x, 0.01f);
		ImGui::Checkbox("isDead", &isDead_);
		ImGui::TreePop();
	}
}

void Enemy::FixedUpdate(Camera* camera_)
{
	worldtransform_.UpdateMatrix();

	attackTimer--;

	if (attackTimer <= 0) {

		if (isDead_ == false) {
			rensya--;

			// 攻撃処理
			if (rensya < 0) {

				if (worldtransform_.translate.z - player_->GetPos().z <= kMaxAttack) {
					Attack();
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

	if (isDeadAnimation_) {
		DeadAnimation();
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

	// 3Dレティクルのワールド座標から2Dレティクルのスクリーン座標を計算
	{
		positionReticle = GetPos();

		// ビューポート行列
		Matrix4x4 matViewport = MakeViewportMatrix(0, 0, (float)WinApp::GetKClientWidth(), (float)WinApp::GetKClientHeight(), 0, 1);

		// ビュー行列とプロジェクション行列、ビューポート行列を合成する
		Matrix4x4 matVPV = Multiply(Multiply(camera_->viewMatrix, camera_->projectionMatrix), matViewport);

		// ワールド→スクリーン座標変換
		positionReticle = Transform(positionReticle, matVPV);

		// スプライトのレティクルに座標設定
		enemySprite_->SetPosition(Vector2(positionReticle.x - 35.0f, positionReticle.y + 0.0f));
	}

	if (isLockOn_) {
		enemySprite_->SetTexture(lockTex);
	}
	else {
		enemySprite_->SetTexture(lockOnTex);
	}
}

void Enemy::FryUpdate(Camera* camera_)
{
	worldtransform_.UpdateMatrix();

	attackTimer--;

	if (attackTimer <= 0) {

		if (isDead_ == false) {
			// 攻撃処理
			if (worldtransform_.translate.z - player_->GetPos().z <= 600.0f) {
				Attack();
			}
		}

		// 発射タイマーを初期化
		attackTimer = kFireInterval;
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

	if (isDeadAnimation_) {
		DeadAnimation();
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

	// 3Dレティクルのワールド座標から2Dレティクルのスクリーン座標を計算
	{
		positionReticle = GetPos();

		// ビューポート行列
		Matrix4x4 matViewport = MakeViewportMatrix(0, 0, (float)WinApp::GetKClientWidth(), (float)WinApp::GetKClientHeight(), 0, 1);

		// ビュー行列とプロジェクション行列、ビューポート行列を合成する
		Matrix4x4 matVPV = Multiply(Multiply(camera_->viewMatrix, camera_->projectionMatrix), matViewport);

		// ワールド→スクリーン座標変換
		positionReticle = Transform(positionReticle, matVPV);

		// スプライトのレティクルに座標設定
		enemySprite_->SetPosition(Vector2(positionReticle.x - 35.0f, positionReticle.y));
	}

	if (isLockOn_) {
		enemySprite_->SetTexture(lockTex);
	}
	else {
		enemySprite_->SetTexture(lockOnTex);
	}
}

void Enemy::Draw(Camera* camera)
{
	if (isDead_ == false) {
		model_->Draw(camera, enemyTex);
	}
	else if (isDead_) {
		//particle_->Draw(camera, enemyBulletTex);
	}

	// 弾の描画
	//bulletPool_.Draw(camera, 0);
}

void Enemy::DrawUI()
{
	if (worldtransform_.translate.z - player_->GetPos().z <= kMaxAttack &&
		worldtransform_.translate.z > player_->GetPos().z)
	{
		if (isDead_ == false) {
			enemySprite_->Draw();
		}
	}
}

void Enemy::Attack()
{
	if (player_ == nullptr || gameScene_ == nullptr) return;

	Vector3 end = player_->GetPos();
	Vector3 start = GetPos();

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

	//// プールから弾を取得
	//EnemyBullet* bullet = bulletPool_.GetBullet();
	//if (bullet) {
	//	bullet->Reset(worldtransform_.translate, velocity_);
	//	bullet->SetPlayer(player_);
	//}
}

void Enemy::OnCollision()
{
	isDeadAnimation_ = true;
	isLockOn_ = false;
	isPossibillityLock = false;
}

void Enemy::DeadAnimation()
{
	// 当たったら回転
	float rotSpeed = 2.0f;
	kRotSpeed += rotSpeed;
	worldtransform_.rotate.y += kRotSpeed;

	// 小さく
	/*float scaleSpeed = 2.0f;
	kScaleSpeed += scaleSpeed;*/
	worldtransform_.scale.x -= kScaleSpeed;
	worldtransform_.scale.y -= kScaleSpeed;
	worldtransform_.scale.z -= kScaleSpeed;

	if (worldtransform_.scale.x <= 0.0f ||
		worldtransform_.scale.y <= 0.0f ||
		worldtransform_.scale.z <= 0.0f) {
		isDead_ = true;
	}


	model_->SetWorldTransform(worldtransform_);
	worldtransform_.UpdateMatrix();
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
