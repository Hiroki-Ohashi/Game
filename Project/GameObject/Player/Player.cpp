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
	kemuri = textureManager_->Load("resources/kemuri.png");

	// レティクル用スプライト
	reticleSprite_ = std::make_unique<Sprite>();
	reticleSprite_->Initialize({ 0.0f,0.0f }, { 50.0f,50.0f }, reticleTex);

	// HP初期化
	isHit_ = false;
	HP = 5;

	// 衝突属性を設定
	SetCollosionAttribute(kcollisionAttributePlayer);
	// 衝突対象を自分の属性以外に設定
	SetCollisionMask(kcollisionAttributeEnemy);

	// emitter
	emitter.count = 10;
	emitter.frequency = 0.016f;
	emitter.frequencyTime = 0.0f;
	emitter.transform.translate = worldtransform_.translate;
	emitter.transform.rotate = worldtransform_.rotate;
	emitter.transform.scale = { 1.0f, 1.0f, 1.0f };

	// particle
	particle_ = std::make_unique<Particles>();
	particle_->Initialize("board.obj", worldtransform_.translate, emitter);
	particle_->SetEmitter(emitter);

	isRot = true;
}

void Player::Update(Camera* camera_)
{
	if (isRot) {
		frame++;
		if (frame >= endFrame) {
			worldtransform_.rotate.z = 0.0f;
			frame = 0;
			isRot = false;
		}
	}

	worldtransform_.rotate.z = easeStart + (easeEnd - easeStart) * EaseOutQuart(frame / endFrame);

	// UI
	uiModel_->Update(camera_);
	uiModel_->SetUIPosition(camera_->cameraTransform.translate);

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

	// エミッタの位置と回転を設定
	emitter.transform.translate = { worldtransform_.translate.x, worldtransform_.translate.y, worldtransform_.translate.z - 10.0f };
	particle_->SetEmitter(emitter);

	// パーティクルを生成して更新
	particle_->Update();
}

void Player::Draw(Camera* camera_)
{
	uiModel_->Draw(camera_, HP);

	if (isHit_) {
		model_->Draw(camera_, hit);
	}
	else if (isHit_ == false){
		model_->Draw(camera_, playerTex);
	}

	if (HP > 0) {
		particle_->Draw(camera_, kemuri);
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

OBB Player::GetOBB() {
	OBB obb;

	// 中心座標はプレイヤーのワールド位置
	obb.center = GetWorldPosition();

	// 半サイズ
	obb.halfSize = GetHalfSize();

	// 各ローカル軸を正規化して格納（行ベース）
	obb.axis[0] = Normalize({ worldtransform_.matWorld.m[0][0], worldtransform_.matWorld.m[0][1], worldtransform_.matWorld.m[0][2] }); // X軸
	obb.axis[1] = Normalize({ worldtransform_.matWorld.m[1][0], worldtransform_.matWorld.m[1][1], worldtransform_.matWorld.m[1][2] }); // Y軸
	obb.axis[2] = Normalize({ worldtransform_.matWorld.m[2][0], worldtransform_.matWorld.m[2][1], worldtransform_.matWorld.m[2][2] }); // Z軸

	return obb;
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

	float t = 1.0f;

	// 引数で受け取った速度をメンバ変数に代入
	velocity_ = Slerp(velocity, worldtransform_.translate, t);

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

	if (HP > 0) {
		// 押した方向で移動ベクトルを変更(左右)
		if (Input::GetInstance()->PushKey(DIK_A)) {
			reticleWorldtransform_.translate.x -= kCharacterSpeed;
		}

		if (Input::GetInstance()->PushKey(DIK_D)) {
			reticleWorldtransform_.translate.x += kCharacterSpeed;
		}

		// 押した方向で移動ベクトルを変更(上下)
		if (Input::GetInstance()->PushKey(DIK_W)) {
			reticleWorldtransform_.translate.y += kCharacterSpeed;
		}
		else if (Input::GetInstance()->PushKey(DIK_S)) {
			reticleWorldtransform_.translate.y -= kCharacterSpeed;
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

			Vector2 stick = Input::GetInstance()->GetLeftStick();

			// Y軸方向の移動だけ
			reticleWorldtransform_.translate.y += stick.y * kCharacterSpeed;

			// 機首の上下
			float targetPitch = -ly * kMaxPitch;

			Vector3 currentRotation = worldtransform_.rotate;
			Vector3 targetRotation = { targetPitch, worldtransform_.rotate.y, targetRoll };

			worldtransform_.rotate = Lerp(currentRotation, targetRotation, 0.5f);
		}
	}

	// 移動限界座標
	const float kMoveLimitX = 180.0f;
	const float kMoveLimitY = 180.0f;

	// 範囲超えない処理
	reticleWorldtransform_.translate.x = max(reticleWorldtransform_.translate.x, -kMoveLimitX);
	reticleWorldtransform_.translate.x = std::min(reticleWorldtransform_.translate.x, +kMoveLimitX);
	reticleWorldtransform_.translate.y = max(reticleWorldtransform_.translate.y, -90.0f);
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
	reticleSprite_->SetPosition(Vector2(positionReticle.x - 25.0f, positionReticle.y - 25.0f));
}

Vector3 Player::PredictPosition(Vector3 shotPosition, Vector3 targetPosition, Vector3 targetPrePosition, float bulletSpeed) {
	// 目標の1フレームの移動速度
	Vector3 velocity = { targetPosition.x - targetPrePosition.x, targetPosition.y - targetPrePosition.y, targetPosition.z - targetPrePosition.z };
	// 射撃位置から見た目標位置
	Vector3 toTarget = { targetPosition.x - shotPosition.x, targetPosition.y - shotPosition.y, targetPosition.z - shotPosition.z };

	float A = (velocity.x * velocity.x + velocity.y * velocity.y + velocity.z * velocity.z) - (bulletSpeed * bulletSpeed);
	float B = 2.0f * (toTarget.x * velocity.x + toTarget.y * velocity.y + toTarget.z * velocity.z);
	float C = (toTarget.x * toTarget.x + toTarget.y * toTarget.y + toTarget.z * toTarget.z);

	if (A == 0) {
		if (B == 0) {
			// 速度がない or すでに当たっている
			return targetPosition;
		}
		else {
			// 未来の衝突時間が負なら、現在の位置をターゲットとする
			float t = -C / B;
			if (t < 0) return targetPosition;
			return { targetPosition.x + velocity.x * t, targetPosition.y + velocity.y * t, targetPosition.z + velocity.z * t };
		}
	}

	float D = B * B - 4 * A * C;
	if (D < 0) {
		// 衝突解なし
		return targetPosition;
	}

	float sqrtD = sqrt(D);
	float t1 = (-B - sqrtD) / (2 * A);
	float t2 = (-B + sqrtD) / (2 * A);
	float timeToHit = (t1 > 0 && t2 > 0) ? std::min(t1, t2) : max(t1, t2);

	// 未来の衝突時間が負なら、現在の位置をターゲットとする
	if (timeToHit < 0) return targetPosition;

	return { targetPosition.x + velocity.x * timeToHit, targetPosition.y + velocity.y * timeToHit, targetPosition.z + velocity.z * timeToHit };
}

void Player::LockOn(Vector3 EnemyPos, Vector3 EnemyPrePos) {

	if (worldtransform_.translate.z < EnemyPos.z &&
		EnemyPos.z - worldtransform_.translate.z <= 2000.0f) {
		// 予測射撃位置を計算
		Vector3 predictedTarget = PredictPosition(worldtransform_.translate, EnemyPos, EnemyPrePos, 60.0f);

		// 自キャラから敵へのベクトル計算
		Vector3 toEnemy = { predictedTarget.x - worldtransform_.translate.x, predictedTarget.y - worldtransform_.translate.y, predictedTarget.z - worldtransform_.translate.z };

		// ベクトルを正規化（単位ベクトル化）
		Vector3 direction = Normalize(toEnemy);

		// 弾の速度を設定
		const float kBulletSpeed = 60.0f;
		Vector3 velocity = { direction.x * kBulletSpeed, direction.y * kBulletSpeed, direction.z * kBulletSpeed };

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

void Player::Attack()
{
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
