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

	model_ = std::make_unique<Model>();
	model_->Initialize("cube.obj", transform_);

	worldtransform_.scale = transform_.scale;
	worldtransform_.rotate = transform_.rotate;
	worldtransform_.translate = transform_.translate;
	worldtransform_.UpdateMatrix();

	playerTex = textureManager_->Load("resources/white.png");
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

	transform_.translate.z += 1.0f;

	// キーボード移動処理
	if (input_->PushKey(DIK_W)) {
		transform_.translate.y += speed;
	}
	if (input_->PushKey(DIK_S)) {
		transform_.translate.y -= speed;
	}
	if (input_->PushKey(DIK_A)) {
		transform_.translate.x -= speed;
	}
	if (input_->PushKey(DIK_D)) {
		transform_.translate.x += speed;
	}

	Attack();

	// 弾更新
	for (PlayerBullet* bullet : bullets_) {
		bullet->Update();
	}

	// 移動限界座標
	const float kMoveLimitX = 16.5f;
	const float kMoveLimitY = 8.5f;

	// 範囲超えない処理
	transform_.translate.x = max(transform_.translate.x, -kMoveLimitX);
	transform_.translate.x = std::min(transform_.translate.x, +kMoveLimitX);
	transform_.translate.y = max(transform_.translate.y, -kMoveLimitY);
	transform_.translate.y = std::min(transform_.translate.y, +kMoveLimitY);

	worldtransform_.translate = transform_.translate;
	worldtransform_.UpdateMatrix();
	model_->SetWorldTransform(worldtransform_);

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

	// 弾描画
	for (PlayerBullet* bullet : bullets_) {
		bullet->Draw(camera_, bulletTex);
	}
}

void Player::Attack()
{
	if (input_->TriggerKey(DIK_SPACE)) {

		// 弾を生成し、初期化
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(transform_.translate);

		// 弾を登録
		bullets_.push_back(newBullet);
	}
}
