#include "Player.h"

/// <summary>
/// Player.cpp
/// プレイヤー生成のソースファイル
/// </summary>


Player::~Player() {
}

void Player::Initialize()
{
	// モデルごとの初期パラメータ
	transform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,25.0f,-250.0f} };
	reticleTransform_ = { {0.2f,0.2f,0.2f},{0.0f,0.0f,0.0f},{transform_.translate.x,transform_.translate.y,transform_.translate.z + 25.0f} };

	// プレイヤー本体モデル
	model_ = std::make_unique<Model>();
	model_->Initialize("player.obj", transform_);
	// プレイヤーワールド座標
	worldtransform_.Initialize();
	worldtransform_.scale = transform_.scale;
	worldtransform_.rotate = transform_.rotate;
	worldtransform_.translate = transform_.translate;
	worldtransform_.UpdateMatrix();

	// レティクル用モデル
	reticleModel_ = std::make_unique<Model>();
	reticleModel_->Initialize("board.obj", reticleTransform_);
	reticleModel_->SetLight(false);
	// レティクルワールド座標
	reticleWorldtransform_.Initialize();
	reticleWorldtransform_.scale = reticleTransform_.scale;
	reticleWorldtransform_.translate = reticleTransform_.translate;
	reticleWorldtransform_.UpdateMatrix();

	// UImodel
	uiModel_ = std::make_unique<PlayerUI>();
	uiModel_->Initialize();

	// texture
	reticleTex = textureManager_->Load("resources/playerReticle.png");
	hit = textureManager_->Load("resources/red.png");
	playerTex = textureManager_->Load("resources/white.png");

	// レティクル用スプライト
	reticleSprite_ = std::make_unique<Sprite>();
	reticleSprite_->Initialize({ 590.0f,310.0f }, { 100.0f,100.0f }, reticleTex);
	reticleSprite_->SetSize({ 100.0f,100.0f });

	// HP初期化
	isHit_ = false;
	HP = 5;

	// 衝突属性を設定
	SetCollosionAttribute(kcollisionAttributePlayer);
	// 衝突対象を自分の属性以外に設定
	SetCollisionMask(kcollisionAttributeEnemy);
}

void Player::Update(Camera* camera_)
{
	uiModel_->Update();
	uiModel_->SetUIPosition(worldtransform_.translate);

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

	// 移動
	Move();

	// レティクルの方向に向きを変える
	PlayerRot();

	// 3Dレティクルのワールド座標から2Dレティクルのスクリーン座標を計算
	Convert2D(camera_);

	// 弾更新
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_) {
		bullet->Update();
	}

	// 当たったら赤く
	if (isHit_) {
		hitTimer_ ++;
		if (hitTimer_ >= 5) {
			hitTimer_ = 0;
			isHit_ = false;
		}
	}
}

void Player::Draw(Camera* camera_)
{
	uiModel_->Draw(camera_);

	if (isHit_) {
		model_->Draw(camera_, hit);
	}
	else if (isHit_ == false){
		model_->Draw(camera_, playerTex);
	}
}

void Player::DrawUI()
{
	if (HP > 0) {
		reticleSprite_->Draw();
	}
}

void Player::BulletDraw(Camera* camera_)
{
	// 弾描画
	for (std::unique_ptr<PlayerBullet> &bullet : bullets_) {
		bullet->Draw(camera_, playerTex);
	}
}

void Player::SetGoalLine(bool goal)
{
	uiModel_->SetEaseEnd(goal);
}

void Player::PlayerRot()
{
	// プレイヤーの向きをレティクルに向ける
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

	// Y軸周り角度（Θy）
	worldtransform_.rotate.y = std::atan2(velocity_.x, velocity_.z);

	float velocityXZ = sqrt((velocity_.x * velocity_.x) + (velocity_.z * velocity_.z));
	worldtransform_.rotate.x = std::atan2(-velocity_.y, velocityXZ);

	if (HP > 0) {
		// 座標移動(ベクトルの加算)
		worldtransform_.translate.x += velocity.x * playerSpeed;
		worldtransform_.translate.y += velocity.y * playerSpeed;
		worldtransform_.translate.z += velocity.z * playerSpeed;
		worldtransform_.UpdateMatrix();

		reticleWorldtransform_.translate.z += playerSpeed;
	}

	// WorldTransformをモデルにセット
	worldtransform_.UpdateMatrix();
	reticleWorldtransform_.UpdateMatrix();

	model_->SetWorldTransform(worldtransform_);
	reticleModel_->SetWorldTransform(reticleWorldtransform_);
}

void Player::Move()
{
	// キャラクターの移動ベクトル
	Vector3 move = { 0, 0, 0 };
	Vector3 rot = { 0, 0, 0 };
	// キャラクターの移動速さ
	const float kCharacterSpeed = 6.0f;
	// 回転速さ[ラジアン/frame]
	//float kRotSpeed = 0.1f;

	if (HP > 0) {
		// 押した方向で移動ベクトルを変更(左右)
		if (Input::GetInstance()->PushKey(DIK_A)) {
			reticleWorldtransform_.translate.x -= kCharacterSpeed;
			//worldtransform_.rotate.y -= kRotSpeed;
		}

		if (Input::GetInstance()->PushKey(DIK_D)) {
			reticleWorldtransform_.translate.x += kCharacterSpeed;
			//worldtransform_.rotate.y += kRotSpeed;
		}

		// 押した方向で移動ベクトルを変更(上下)
		if (Input::GetInstance()->PushKey(DIK_W)) {
			reticleWorldtransform_.translate.y += kCharacterSpeed;
			//worldtransform_.rotate.x -= kRotSpeed;
		}
		else if (Input::GetInstance()->PushKey(DIK_S)) {
			reticleWorldtransform_.translate.y -= kCharacterSpeed;
			//worldtransform_.rotate.x += kRotSpeed;
		}

		// ゲームパッドの状態を得る変数(XINPUT)
		XINPUT_STATE joyState;

		// ゲームパッド状態取得
		if (Input::GetInstance()->GetJoystickState(joyState)) {
			reticleWorldtransform_.translate.x += (float)joyState.Gamepad.sThumbLX / SHRT_MAX * kCharacterSpeed;
			reticleWorldtransform_.translate.y += (float)joyState.Gamepad.sThumbLY / SHRT_MAX * kCharacterSpeed;

			float lx = (float)joyState.Gamepad.sThumbLX / SHRT_MAX;
			float ly = (float)joyState.Gamepad.sThumbLY / SHRT_MAX;

			// 機体を左右に傾ける
			float targetRoll = -lx * kMaxRoll;
			worldtransform_.rotate.z = worldtransform_.rotate.z * (1.0f - kRollLerpFactor) + targetRoll * kRollLerpFactor;

			// 機首の上下
			float targetPitch = ly * kMaxPitch;
			worldtransform_.rotate.x = worldtransform_.rotate.x * (1.0f - kPitchLerpFactor) + targetPitch * kPitchLerpFactor;

			// ロール角に応じて旋回を調整
			float yawSpeed = sinf(worldtransform_.rotate.z) * kYawSpeed;
			worldtransform_.rotate.y += yawSpeed;
		}
	}

	// 移動限界座標
	const float kMoveLimitX = 140.0f;
	const float kMoveLimitY = 120.0f;

	// 範囲超えない処理
	reticleWorldtransform_.translate.x = max(reticleWorldtransform_.translate.x, -kMoveLimitX);
	reticleWorldtransform_.translate.x = std::min(reticleWorldtransform_.translate.x, +kMoveLimitX);
	reticleWorldtransform_.translate.y = max(reticleWorldtransform_.translate.y, -100.0f);
	reticleWorldtransform_.translate.y = std::min(reticleWorldtransform_.translate.y, +kMoveLimitY);

	// WorldTransformをモデルにセット
	worldtransform_.UpdateMatrix();
	reticleWorldtransform_.UpdateMatrix();

	model_->SetWorldTransform(worldtransform_);
	reticleModel_->SetWorldTransform(reticleWorldtransform_);
}

void Player::Convert2D(Camera* camera_)
{
	// 3Dレティクルのワールド座標から2Dレティクルのスクリーン座標を計算
	positionReticle = Get3DWorldPosition();

	// ビューポート行列
	Matrix4x4 matViewport = MakeViewportMatrix(0, 0, (float)WinApp::GetKClientWidth(), (float)WinApp::GetKClientHeight(), 0, 1);

	// ビュー行列とプロジェクション行列、ビューポート行列を合成する
	Matrix4x4 matVPV = Multiply(Multiply(camera_->viewMatrix, camera_->projectionMatrix), matViewport);

	// ワールド→スクリーン座標変換
	positionReticle = Transform(positionReticle, matVPV);

	// スプライトのレティクルに座標設定
	reticleSprite_->SetPosition(Vector2(positionReticle.x - 50.0f, positionReticle.y - 50.0f));
}

void Player::LockOn(Vector3 EnemyPos)
{
	XINPUT_STATE joyState{};

	if (worldtransform_.translate.z < EnemyPos.z &&
		EnemyPos.z - worldtransform_.translate.z <= 600.0f) {

		if (Input::GetInstance()->GetJoystickState(joyState)) {
			// Aボタンが押された場合のみ処理を実行
			if (Input::GetInstance()->PressedButton(joyState, XINPUT_GAMEPAD_A)) {
				// ターゲットの位置
				Vector3 end = EnemyPos;
				// プレイヤーの位置
				Vector3 start = worldtransform_.translate;

				// 自キャラから敵へのベクトル計算
				Vector3 toEnemy = { end.x - start.x, end.y - start.y, end.z - start.z };

				// ベクトルを正規化（単位ベクトル化）
				Vector3 direction = Normalize(toEnemy);

				// 弾の進行方向をターゲット方向に強く補正
				float homingFactor = 5.0f;  // ホーミングの強さ
				direction.x += direction.x * homingFactor;
				direction.y += direction.y * homingFactor;
				direction.z += direction.z * homingFactor;

				// 弾の速度を設定
				const float kBulletSpeed = 10.0f;
				Vector3 velocity(direction.x * kBulletSpeed, direction.y * kBulletSpeed, direction.z * kBulletSpeed);

				// 回転を計算
				worldtransform_.rotate.y = std::atan2(velocity.x, velocity.z);
				float velocityXZ = sqrt((velocity.x * velocity.x) + (velocity.z * velocity.z));
				worldtransform_.rotate.x = std::atan2(-velocity.y, velocityXZ);

				// 弾を生成し、初期化
				std::unique_ptr<PlayerBullet> newBullet = std::make_unique<PlayerBullet>();
				newBullet->Initialize(worldtransform_.translate, velocity);

				// 弾を登録
				bullets_.push_back(std::move(newBullet));
			}
		}
	}
}

void Player::Attack()
{
	XINPUT_STATE joyState{};
	if (Input::GetInstance()->GetJoystickState(joyState)) {
		if (Input::GetInstance()->PressedButton(joyState, XINPUT_GAMEPAD_A)) {
			// 弾の速度
			const float kBulletSpeed = 60.0f;

			// 自機から照準オブジェクトへのベクトルを計算
			Vector3 targetDirection;
			targetDirection.x = reticleWorldtransform_.translate.x - worldtransform_.translate.x;
			targetDirection.y = reticleWorldtransform_.translate.y - worldtransform_.translate.y;
			targetDirection.z = reticleWorldtransform_.translate.z - worldtransform_.translate.z;

			// 正規化して速度を設定
			Vector3 velocity;
			velocity.x = Normalize(targetDirection).x * kBulletSpeed;
			velocity.y = Normalize(targetDirection).y * kBulletSpeed;
			velocity.z = Normalize(targetDirection).z * kBulletSpeed;

			// 弾を生成し、初期化
			std::unique_ptr<PlayerBullet> newBullet = std::make_unique<PlayerBullet>();
			newBullet->Initialize(worldtransform_.translate, velocity);

			// 弾を登録
			bullets_.push_back(std::move(newBullet));
		}
	}
}

void Player::OnCollision()
{
	isHit_ = true;
	HP -= damage_;
}

Vector3 Player::GetWorldPosition() const
{
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldtransform_.matWorld.m[3][0];
	worldPos.y = worldtransform_.matWorld.m[3][1];
	worldPos.z = worldtransform_.matWorld.m[3][2];

	return worldPos;
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
