#include "Player.h"

Player::~Player() {
	// bullet_の解放
	for (PlayerBullet* bullet : bullets_) {
		delete bullet;
	}
}

void Player::Initialize()
{
	transform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	reticleTransform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{transform_.translate.x,transform_.translate.y,transform_.translate.z + 50.0f} };

	model_ = std::make_unique<Model>();
	model_->Initialize("cube.obj", transform_);

	reticleModel_ = std::make_unique<Model>();
	reticleModel_->Initialize("plane.obj", reticleTransform_);

	worldtransform_.scale = transform_.scale;
	worldtransform_.rotate = transform_.rotate;
	worldtransform_.translate = transform_.translate;
	worldtransform_.UpdateMatrix();

	reticleWorldtransform_.scale = reticleTransform_.scale;
	reticleWorldtransform_.rotate = reticleTransform_.rotate;
	reticleWorldtransform_.translate = reticleTransform_.translate;
	reticleWorldtransform_.UpdateMatrix();

	playerTex = textureManager_->Load("resources/white.png");
	reticleTex = textureManager_->Load("resources/reticle.png");
	bulletTex = textureManager_->Load("resources/white.png");
}

void Player::Update()
{
	// デスフラグの立った弾を排除
	bullets_.remove_if([](PlayerBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

	// キャラクターの移動ベクトル
	Vector3 move = { 0, 0, 0 };
	// キャラクターの移動速さ
	const float kCharacterSpeed = 0.2f;
	// 回転速さ[ラジアン/frame]
	const float kRotSpeed = 0.02f;

	// 押した方向で移動ベクトルを変更(左右)
	if (input_->PushKey(DIK_A)) {
		move.x -= kCharacterSpeed;
	}
	else if (input_->PushKey(DIK_D)) {
		move.x += kCharacterSpeed;
	}
	// 押した方向で移動ベクトルを変更(上下)
	if (input_->PushKey(DIK_W)) {
		move.y -= kCharacterSpeed;
	}
	else if (input_->PushKey(DIK_S)) {
		move.y += kCharacterSpeed;
	}

	// 座標移動(ベクトルの加算)
	worldtransform_.translate.x += move.x;
	worldtransform_.translate.y -= move.y;

	// 移動限界座標
	const float kMoveLimitX = 20.0f;
	const float kMoveLimitY = 20.0f;

	// 範囲超えない処理
	worldtransform_.translate.x = max(worldtransform_.translate.x, -kMoveLimitX);
	worldtransform_.translate.x = std::min(worldtransform_.translate.x, +kMoveLimitX);
	worldtransform_.translate.y = max(worldtransform_.translate.y, -kMoveLimitY);
	worldtransform_.translate.y = std::min(worldtransform_.translate.y, +kMoveLimitY);
	worldtransform_.UpdateMatrix();

	// 自機から3Dレティクルへの距離
	const float kDistancePlayerTo3DReticle = 50.0f;
	// 自機から3Dレティクルへのオフセット(Z+向き)
	Vector3 offset = { 0, 0, 1.0f };
	// 自機のワールド行列の回転を反映
	offset = Transforme(offset, worldtransform_.matWorld);
	// ベクトルの長さを変える
	offset.x = Normalize(offset).x * kDistancePlayerTo3DReticle;
	offset.y = Normalize(offset).y * kDistancePlayerTo3DReticle;
	offset.z = Normalize(offset).z * kDistancePlayerTo3DReticle;
	// 3Dレティクルの座標を設定
	reticleWorldtransform_.translate.x = worldtransform_.translate.x + offset.x;
	reticleWorldtransform_.translate.y = worldtransform_.translate.y + offset.y;
	reticleWorldtransform_.translate.z = worldtransform_.translate.z + offset.z;
	reticleWorldtransform_.UpdateMatrix();

	worldtransform_.translate.z += 0.5f;
	reticleWorldtransform_.translate.z += 0.5f;

	model_->SetWorldTransform(worldtransform_);
	reticleModel_->SetWorldTransform(reticleWorldtransform_);

	// 攻撃処理
	Attack();

	// 弾更新
	for (PlayerBullet* bullet : bullets_) {
		bullet->Update();
	}

	if (ImGui::TreeNode("Player")) {
		ImGui::DragFloat3("Rotate.y ", &worldtransform_.rotate.x, 0.01f);
		ImGui::DragFloat3("Transform", &worldtransform_.translate.x, 0.01f);

		//ImGui::DragFloat2("UVTransform", &worldtransform_.translate.x, 0.01f, -10.0f, 10.0f);
		//ImGui::DragFloat2("UVScale", &worldtransform_.scale.x, 0.01f, -10.0f, 10.0f);
		//ImGui::SliderAngle("UVRotate", &worldtransform_.rotate.z);
		ImGui::TreePop();
	}
}

void Player::Draw(Camera* camera_)
{
	model_->Draw(camera_, bulletTex);
	reticleModel_->Draw(camera_, reticleTex);

	// 弾描画
	for (PlayerBullet* bullet : bullets_) {
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
		const float kBulletSpeed = 2.0f;
		Vector3 velocity(0, 0, kBulletSpeed);

		// 速度ベクトルを自機の向きに併せて回転させる
		velocity = Transforme(velocity, worldtransform_.matWorld);
		velocity = Transforme(velocity, worldtransform_.matWorld);

		// 自機から照準オブジェクトへのベクトル
		velocity.x = Get3DWorldPosition().x - GetPos().x;
		velocity.y = Get3DWorldPosition().y - GetPos().y;
		velocity.z = Get3DWorldPosition().z - GetPos().z;

		velocity.x = Normalize(velocity).x * kBulletSpeed;
		velocity.y = Normalize(velocity).y * kBulletSpeed;
		velocity.z = Normalize(velocity).z * kBulletSpeed;

		// 弾を生成し、初期化
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(worldtransform_.translate, velocity);

		// 弾を登録
		bullets_.push_back(newBullet);
	}
}
