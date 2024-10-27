#include "Player.h"

/// <summary>
/// Player.cpp
/// プレイヤー生成のソースファイル
/// </summary>


Player::~Player() {
}

void Player::Initialize()
{
	transform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,25.0f,250.0f} };
	reticleTransform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{transform_.translate.x,transform_.translate.y,transform_.translate.z + 40.0f} };

	model_ = std::make_unique<Model>();
	model_->Initialize("player.obj", transform_);

	reticleModel_ = std::make_unique<Model>();
	reticleModel_->Initialize("board.obj", reticleTransform_);

	worldtransform_.Initialize();
	worldtransform_.scale = transform_.scale;
	worldtransform_.rotate = transform_.rotate;
	worldtransform_.translate = transform_.translate;
	worldtransform_.UpdateMatrix();

	reticleWorldtransform_.Initialize();
	reticleWorldtransform_.scale = reticleTransform_.scale;
	reticleWorldtransform_.rotate = reticleTransform_.rotate;
	reticleWorldtransform_.translate = reticleTransform_.translate;
	reticleWorldtransform_.UpdateMatrix();

	playerTex = textureManager_->Load("resources/white.png");
	reticleTex = textureManager_->Load("resources/reticle.png");
	bulletTex = textureManager_->Load("resources/white.png");
	hit = textureManager_->Load("resources/red.png");

	isHit_ = false;
}

void Player::Update()
{
	// デスフラグの立った弾を排除
	bullets_.erase(
		std::remove_if(
			bullets_.begin(),
			bullets_.end(),
			[](const std::unique_ptr<PlayerBullet>& bullet) {
				return bullet->IsDead();
			}
		),
		bullets_.end()
	);

	// キャラクターの移動ベクトル
	Vector3 move = { 0, 0, 0 };
	Vector3 rot = { 0, 0, 0 };
	// キャラクターの移動速さ
	const float kCharacterSpeed = 0.5f;
	// 回転速さ[ラジアン/frame]
	float kRotSpeed = 0.01f;

	// 押した方向で移動ベクトルを変更(左右)
	if (input_->PushKey(DIK_A)) {
		move.x -= kCharacterSpeed;
		worldtransform_.rotate.y -= kRotSpeed;
	}
	
	if (input_->PushKey(DIK_D)) {
		move.x += kCharacterSpeed;
		worldtransform_.rotate.y += kRotSpeed;
	}

	// 押した方向で移動ベクトルを変更(上下)
	if (input_->PushKey(DIK_W)) {
		move.y -= kCharacterSpeed;
		worldtransform_.rotate.x -= kRotSpeed;
	}
	else if (input_->PushKey(DIK_S)) {
		move.y += kCharacterSpeed;
		worldtransform_.rotate.x += kRotSpeed;
	}

	// ゲームパッドの状態を得る変数(XINPUT)
	XINPUT_STATE joyState;

	// ゲームパッド状態取得
	if (Input::GetInsTance()->GetJoystickState(joyState)) {
		reticleWorldtransform_.translate.x += (float)joyState.Gamepad.sThumbLX / SHRT_MAX * kCharacterSpeed;
		reticleWorldtransform_.translate.y += (float)joyState.Gamepad.sThumbLY / SHRT_MAX * kCharacterSpeed;
	}

	reticleWorldtransform_.UpdateMatrix();

	Vector3 end = reticleWorldtransform_.translate;
	Vector3 start = worldtransform_.translate;

	Vector3 diff;
	diff.x = end.x - start.x;
	diff.y = end.y - start.y;
	diff.z = end.z - start.z;

	diff = Normalize(diff);

	Vector3 velocity(diff.x, diff.y, diff.z);

	float t = 0.0f;

	// 引数で受け取った速度をメンバ変数に代入
	velocity_ = Slerp(velocity, worldtransform_.translate, t);

	velocity_.x *= 0.1f;
	velocity_.y *= 0.1f;
	velocity_.z *= 0.1f;

	// Y軸周り角度（Θy）
	worldtransform_.rotate.y = std::atan2(velocity_.x, velocity_.z);

	float velocityXZ = sqrt((velocity_.x * velocity_.x) + (velocity_.z * velocity_.z));
	worldtransform_.rotate.x = std::atan2(-velocity_.y, velocityXZ);

	// 座標移動(ベクトルの加算)
	//worldtransform_.translate.x += velocity.x * 1.5f;
	//worldtransform_.translate.y += velocity.y * 1.5f;
	//worldtransform_.translate.z += velocity.z * 1.5f;
	worldtransform_.UpdateMatrix();

	//reticleWorldtransform_.translate.z += 1.5f;

	model_->SetWorldTransform(worldtransform_);
	reticleModel_->SetWorldTransform(reticleWorldtransform_);

	// 攻撃処理
	Attack();

	// 弾更新
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_) {
		bullet->Update();
	}

	if (isHit_) {
		hitTimer_ ++;
		if (hitTimer_ >= 5) {
			hitTimer_ = 0;
			isHit_ = false;
		}
	}

	if (ImGui::TreeNode("Player")) {
		ImGui::DragFloat3("Rotate.y ", &worldtransform_.rotate.x, 0.01f);
		ImGui::DragFloat3("Transform", &worldtransform_.translate.x, 0.01f);
		ImGui::Text("PreyerState = %d", hitTimer_);
		ImGui::TreePop();
	}
}

void Player::Draw(Camera* camera_)
{
	if (isHit_) {
		model_->Draw(camera_, hit);
	}
	else if (isHit_ == false){
		model_->Draw(camera_, playerTex);
	}

	reticleModel_->Draw(camera_, reticleTex);
}

void Player::BulletDraw(Camera* camera_)
{

	// 弾描画
	for (std::unique_ptr<PlayerBullet> &bullet : bullets_) {
		bullet->Draw(camera_, bulletTex);
	}
}

Vector3 Player::Get3DWorldPosition()
{
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得
	worldPos.x = reticleWorldtransform_.matWorld.m[3][0];
	worldPos.y = reticleWorldtransform_.matWorld.m[3][1];
	worldPos.z = reticleWorldtransform_.matWorld.m[3][2];

	return worldPos;
}

void Player::Attack()
{
	if (input_->TriggerKey(DIK_SPACE)) {

		// 弾の速度
		const float kBulletSpeed = 5.0f;
		Vector3 velocity(0, 0, kBulletSpeed);

		// 速度ベクトルを自機の向きに併せて回転させる
		velocity = TransformNormal(velocity, worldtransform_.matWorld);

		// 自機から照準オブジェクトへのベクトル
		velocity.x = Get3DWorldPosition().x - GetPos().x;
		velocity.y = Get3DWorldPosition().y - GetPos().y;
		velocity.z = Get3DWorldPosition().z - GetPos().z;

		velocity.x = Normalize(velocity).x * kBulletSpeed;
		velocity.y = Normalize(velocity).y * kBulletSpeed;
		velocity.z = Normalize(velocity).z * kBulletSpeed;

		// 弾を生成し、初期化
		std::unique_ptr<PlayerBullet> newBullet = std::make_unique<PlayerBullet>();
		newBullet->Initialize(worldtransform_.translate, velocity);

		// 弾を登録
		bullets_.push_back(std::move(newBullet));
	}

	XINPUT_STATE joyState{};
	if (Input::GetInsTance()->GetJoystickState(joyState)) {
		if (Input::GetInsTance()->PressedButton(joyState, XINPUT_GAMEPAD_A)) {
			// 弾の速度
			const float kBulletSpeed = 10.0f;
			Vector3 velocity(0, 0, kBulletSpeed);

			// 速度ベクトルを自機の向きに併せて回転させる
			velocity = TransformNormal(velocity, worldtransform_.matWorld);

			// 自機から照準オブジェクトへのベクトル
			velocity.x = Get3DWorldPosition().x - GetPos().x;
			velocity.y = Get3DWorldPosition().y - GetPos().y;
			velocity.z = Get3DWorldPosition().z - GetPos().z;

			velocity.x = Normalize(velocity).x * kBulletSpeed;
			velocity.y = Normalize(velocity).y * kBulletSpeed;
			velocity.z = Normalize(velocity).z * kBulletSpeed;

			// 弾を生成し、初期化
			std::unique_ptr<PlayerBullet> newBullet = std::make_unique<PlayerBullet>();
			newBullet->Initialize(worldtransform_.translate, velocity);

			// 弾を登録
			bullets_.push_back(std::move(newBullet));
		}
	}
}
