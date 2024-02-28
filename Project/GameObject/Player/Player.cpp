#include "Player.h"

Player::~Player() {
	// bullet_の解放
	for (PlayerBullet* bullet : bullets_) {
		delete bullet;
	}
}

void Player::Init() {
	transform = { { 0.1f,0.1f,0.1f},{0.0f,0.0f,0.0f},{0.01f,0.01f,0.01f} };
	transform2 = { { 0.1f,0.1f,0.1f},{0.0f,0.0f,0.0f},{0.01f,0.01f,0.01f} };

	model_ = std::make_unique<Model>();
	model_->Initialize("cube.obj", transform);

	model_2 = std::make_unique<Model>();
	model_2->Initialize("pro.obj", transform2);
}

void Player::Update() {

	// デスフラグんお立った弾を排除
	bullets_.remove_if([](PlayerBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

	transform.translate.z -= 0.05f;
	transform2.translate.z -= 0.05f;

	model_2->worldTransform_.rotate.y += 0.3f;

	model_->worldTransform_.translate = transform.translate;
	model_2->worldTransform_.translate = transform2.translate;

	if (input_->PushKey(DIK_W)) {
		transform.translate.y += speed;
		transform2.translate.y += speed;
	}
	if (input_->PushKey(DIK_S)) {
		transform.translate.y -= speed;
		transform2.translate.y -= speed;
	}
	if (input_->PushKey(DIK_A)) {
		transform.translate.x -= speed;
		transform2.translate.x -= speed;
	}
	if (input_->PushKey(DIK_D)) {
		transform.translate.x += speed;
		transform2.translate.x += speed;
	}

	// 移動限界座標
	const float kMoveLimitX = 1.9f;
	const float kMoveLimitY = 1.0f;

	// 範囲超えない処理
	transform.translate.x = max(transform.translate.x, -kMoveLimitX);
	transform.translate.x = min(transform.translate.x, +kMoveLimitX);
	transform.translate.y = max(transform.translate.y, -kMoveLimitY);
	transform.translate.y = min(transform.translate.y, +kMoveLimitY);

	transform2.translate.x = max(transform.translate.x, -kMoveLimitX);
	transform2.translate.x = min(transform.translate.x, +kMoveLimitX);
	transform2.translate.y = max(transform.translate.y, -kMoveLimitY);
	transform2.translate.y = min(transform.translate.y, +kMoveLimitY);

	Attack();

	// 弾更新
	for (PlayerBullet* bullet : bullets_) {
		bullet->Update();
	}

}

void Player::Draw(uint32_t index, Camera* camera, uint32_t index2){
	model_->Draw(camera, index);
	model_2->Draw(camera, index);

	// 弾描画
	for (PlayerBullet* bullet : bullets_) {
		bullet->Draw(camera, index2);
	}
}

void Player::Attack() {
	if (input_->TriggerKey(DIK_SPACE)) {

		// 弾を生成し、初期化
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Init(transform.translate);

		// 弾を登録
		bullets_.push_back(newBullet);
	}
}

void Player::OnCollision()
{
}
