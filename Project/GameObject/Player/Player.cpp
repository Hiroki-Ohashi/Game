#include "Player.h"

/// <summary>
/// Player.cpp
/// プレイヤー生成のソースファイル
/// </summary>


Player::~Player() {
}

void Player::Initialize()
{
	transform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,25.0f,-250.0f} };
	reticleTransform_ = { {0.5f,0.5f,0.5f},{0.0f,0.0f,0.0f},{transform_.translate.x,transform_.translate.y,transform_.translate.z + 25.0f} };

	model_ = std::make_unique<Model>();
	model_->Initialize("player.obj", transform_);

	reticleModel_ = std::make_unique<Model>();
	reticleModel_->Initialize("board.obj", reticleTransform_);
	reticleModel_->SetLight(false);

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
	hit = textureManager_->Load("resources/red.png");

	reticleSprite_ = std::make_unique<Sprite>();
	reticleSprite_->Initialize({ 590.0f,310.0f }, { 100.0f,100.0f }, reticleTex);
	reticleSprite_->SetSize({ 100.0f,100.0f });

	hp5 = textureManager_->Load("resources/hp5.png");
	hp4 = textureManager_->Load("resources/hp4.png");
	hp3 = textureManager_->Load("resources/hp3.png");
	hp2 = textureManager_->Load("resources/hp2.png");
	hp1 = textureManager_->Load("resources/hp1.png");
	hp0 = textureManager_->Load("resources/hp0.png");

	hp0_ = std::make_unique<Sprite>();
	hp0_->Initialize({ 0.0f,0.0f }, { 1.0f,1.0f }, hp0);
	hp1_ = std::make_unique<Sprite>();
	hp1_->Initialize({ 0.0f,0.0f }, { 1.0f,1.0f }, hp1);
	hp2_ = std::make_unique<Sprite>();
	hp2_->Initialize({ 0.0f,0.0f }, { 1.0f,1.0f }, hp2);
	hp3_ = std::make_unique<Sprite>();
	hp3_->Initialize({ 0.0f,0.0f }, { 1.0f,1.0f }, hp3);
	hp4_ = std::make_unique<Sprite>();
	hp4_->Initialize({ 0.0f,0.0f }, { 1.0f,1.0f }, hp4);
	hp5_ = std::make_unique<Sprite>();
	hp5_->Initialize({ 0.0f,0.0f }, { 1.0f,1.0f }, hp5);

	isHit_ = false;
	HP = 5;
}

void Player::Update(Camera* camera_)
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
	const float kCharacterSpeed = 1.0f;
	// 回転速さ[ラジアン/frame]
	float kRotSpeed = 0.1f;

	// 押した方向で移動ベクトルを変更(左右)
	if (input_->PushKey(DIK_A)) {
		reticleWorldtransform_.translate.x -= kCharacterSpeed;
		//worldtransform_.rotate.y -= kRotSpeed;
	}
	
	if (input_->PushKey(DIK_D)) {
		reticleWorldtransform_.translate.x += kCharacterSpeed;
		//worldtransform_.rotate.y += kRotSpeed;
	}

	// 押した方向で移動ベクトルを変更(上下)
	if (input_->PushKey(DIK_W)) {
		reticleWorldtransform_.translate.y += kCharacterSpeed;
		//worldtransform_.rotate.x -= kRotSpeed;
	}
	else if (input_->PushKey(DIK_S)) {
		reticleWorldtransform_.translate.y -= kCharacterSpeed;
		//worldtransform_.rotate.x += kRotSpeed;
	}

	// ゲームパッドの状態を得る変数(XINPUT)
	XINPUT_STATE joyState;

	// ゲームパッド状態取得
	if (Input::GetInsTance()->GetJoystickState(joyState)) {
		reticleWorldtransform_.translate.x += (float)joyState.Gamepad.sThumbLX / SHRT_MAX * kCharacterSpeed;
		reticleWorldtransform_.translate.y += (float)joyState.Gamepad.sThumbLY / SHRT_MAX * kCharacterSpeed;

		worldtransform_.rotate.z -= (float)joyState.Gamepad.sThumbLX / SHRT_MAX * kRotSpeed;
	}

	reticleWorldtransform_.UpdateMatrix();

	//// ゲームパッドの状態を得る変数(XINPUT)
	//XINPUT_STATE joyState;

	//Matrix4x4 matViewport = MakeViewportMatrix(0.0f, 0.0f, (float)WinApp::GetKClientWidth(), (float)WinApp::GetKClientHeight(), 0.0f, 1.0f);

	//// スプライトの現在座標を取得
	//Vector2 spritePosition = { reticleSprite_->GetPos().x, reticleSprite_->GetPos().y};

	//// ゲームパッド状態取得
	//if (Input::GetInsTance()->GetJoystickState(joyState)) {
	//	spritePosition.x += (float)joyState.Gamepad.sThumbLX / SHRT_MAX * kCharacterSpeed;
	//	spritePosition.y -= (float)joyState.Gamepad.sThumbLY / SHRT_MAX * kCharacterSpeed;

	//	//worldtransform_.rotate.z -= (float)joyState.Gamepad.sThumbLX / SHRT_MAX * kRotSpeed;

	//	reticleSprite_->SetPosition(spritePosition);
	//}

	//// ビュープロジェクションビューポート合成行列
	//Matrix4x4 matVPV = Multiply(Multiply(camera_->viewMatrix, camera_->projectionMatrix), matViewport);

	//// 合成行列の逆行列を計算する
	//Matrix4x4 matInverseVPV = Inverse(matVPV);

	//// スクリーン座標
	//Vector3 posNear = Vector3(static_cast<float>(spritePosition.x), (float)spritePosition.y, 0);
	//Vector3 posFar = Vector3(static_cast<float>(spritePosition.x), float(spritePosition.y), 1);
	//// スクリーン座標系からワールド座標系へ
	//posNear = Transform(posNear, matInverseVPV);
	//posFar = Transform(posFar, matInverseVPV);

	//// スティックレイの方向
	//Vector3 spriteDierection;
	//spriteDierection.x = posFar.x - posNear.x;
	//spriteDierection.y = posFar.y - posNear.y;
	//spriteDierection.z = posFar.z - posNear.z;
	//spriteDierection = Normalize(spriteDierection);
	//// カメラから照準オブジェクトの距離
	//const float kDistanceTextObject = 100.0f;
	//reticleWorldtransform_.translate.y = spriteDierection.y * kDistanceTextObject;
	//reticleWorldtransform_.translate.z = spriteDierection.z * kDistanceTextObject;
	//reticleWorldtransform_.translate.x = spriteDierection.x * kDistanceTextObject;

	//reticleWorldtransform_.UpdateMatrix();

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

	velocity_.x *= 0.1f;
	velocity_.y *= 0.1f;
	velocity_.z *= 0.1f;

	// Y軸周り角度（Θy）
	worldtransform_.rotate.y = std::atan2(velocity_.x, velocity_.z);

	float velocityXZ = sqrt((velocity_.x * velocity_.x) + (velocity_.z * velocity_.z));
	worldtransform_.rotate.x = std::atan2(-velocity_.y, velocityXZ);

	//// 座標移動(ベクトルの加算)
	worldtransform_.translate.x += velocity.x * 1.52f;
	worldtransform_.translate.y += velocity.y * 1.52f;
	worldtransform_.translate.z += velocity.z * 1.52f;
	worldtransform_.UpdateMatrix();

	reticleWorldtransform_.translate.z += 1.5f;

	model_->SetWorldTransform(worldtransform_);
	reticleModel_->SetWorldTransform(reticleWorldtransform_);

	// 移動限界座標
	const float kMoveLimitX = 80.9f;
	const float kMoveLimitY = 100.0f;

	// 範囲超えない処理
	reticleWorldtransform_.translate.x = max(reticleWorldtransform_.translate.x, -kMoveLimitX);
	reticleWorldtransform_.translate.x = std::min(reticleWorldtransform_.translate.x, +kMoveLimitX);
	reticleWorldtransform_.translate.y = max(reticleWorldtransform_.translate.y, -7.0f);
	reticleWorldtransform_.translate.y = std::min(reticleWorldtransform_.translate.y, +kMoveLimitY);

	worldtransform_.rotate.z = max(worldtransform_.rotate.z, -1.0f);
	worldtransform_.rotate.z = std::min(worldtransform_.rotate.z, +1.0f);

	// 3Dレティクルのワールド座標から2Dレティクルのスクリーン座標を計算
	{
		Vector3 positionReticle = Get3DWorldPosition();

		// ビューポート行列
		Matrix4x4 matViewport = MakeViewportMatrix(0, 0, (float)WinApp::GetKClientWidth(), (float)WinApp::GetKClientHeight(), 0, 1);

		// ビュー行列とプロジェクション行列、ビューポート行列を合成する
		Matrix4x4 matVPV = Multiply(Multiply(camera_->viewMatrix, camera_->projectionMatrix), matViewport);

		// ワールド→スクリーン座標変換
		positionReticle = Transform(positionReticle, matVPV);

		// スプライトのレティクルに座標設定
		reticleSprite_->SetPosition(Vector2(positionReticle.x - 50, positionReticle.y - 50));
	}

	//// 攻撃処理
	//for (std::unique_ptr<Enemy>& enemy : GetEnemys()) {
	//	if (enemy->GetIsLockOn()) {
	//		LockOn(enemy); // ロックオン処理
	//		break;
	//	}
	//}

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
		ImGui::DragFloat3("Transform", &worldtransform_.translate.x, 0.1f);
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

	//reticleModel_->Draw(camera_, reticleTex);
}

void Player::DrawUI()
{
	reticleSprite_->Draw();

	if (GetHP() == 5) {
		hp5_->Draw();
	}
	else if (GetHP() == 4) {
		hp4_->Draw();
	}
	else if (GetHP() == 3) {
		hp3_->Draw();
	}
	else if (GetHP() == 2) {
		hp2_->Draw();
	}
	else if (GetHP() == 1) {
		hp1_->Draw();
	}
	else if (GetHP() == 0) {
		hp0_->Draw();
	}
}

void Player::BulletDraw(Camera* camera_)
{
	// 弾描画
	for (std::unique_ptr<PlayerBullet> &bullet : bullets_) {
		bullet->Draw(camera_, playerTex);
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

void Player::Attack()
{
	if (input_->TriggerKey(DIK_SPACE)) {

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

	XINPUT_STATE joyState{};
	if (Input::GetInsTance()->GetJoystickState(joyState)) {
		if (Input::GetInsTance()->PressedButton(joyState, XINPUT_GAMEPAD_A)) {
			// 弾の速度
			const float kBulletSpeed = 10.0f;

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

void Player::LockOn(std::unique_ptr<Enemy>& enemy)
{
	XINPUT_STATE joyState{};
	if (Input::GetInsTance()->GetJoystickState(joyState)) {
		// Aボタンが押された場合のみ処理を実行
		if (Input::GetInsTance()->PressedButton(joyState, XINPUT_GAMEPAD_A)) {
			// ターゲットの位置
			Vector3 end = enemy->GetPos();
			// プレイヤーの位置
			Vector3 start = worldtransform_.translate;

			// ターゲットまでのベクトルを計算
			Vector3 diff;
			diff.x = end.x - start.x;
			diff.y = end.y - start.y;
			diff.z = end.z - start.z;

			// 正規化して方向ベクトルにする
			diff = Normalize(diff);

			// 弾の速度を設定
			const float kBulletSpeed = 10.0f;
			Vector3 velocity(diff.x * kBulletSpeed, diff.y * kBulletSpeed, diff.z * kBulletSpeed);

			// 弾を生成し、初期化
			std::unique_ptr<PlayerBullet> newBullet = std::make_unique<PlayerBullet>();
			newBullet->Initialize(worldtransform_.translate, velocity);

			// 弾を登録
			bullets_.push_back(std::move(newBullet));
		}
	}
}
