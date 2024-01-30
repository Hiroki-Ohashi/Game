#include "Player.h"

Player::~Player() {
	// bullet_の解放
	for (PlayerBullet* bullet : bullets_) {
		delete bullet;
	}

	delete model_;
}

void Player::Init(const std::string filename) {
	transform = { { 0.1f,0.1f,0.1f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	model_ = new Model();
	model_->Initialize(filename, transform);

	bulletTex = textureManager_->Load("Resources/kusa.png");
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

	model_->worldTransform_.translate = transform.translate;

	if (input_->PushKey(DIK_W)) {
		transform.translate.y += speed;
	}
	if (input_->PushKey(DIK_S)) {
		transform.translate.y -= speed;
	}
	if (input_->PushKey(DIK_A)) {
		transform.translate.x -= speed;
	}
	if (input_->PushKey(DIK_D)) {
		transform.translate.x += speed;
	}

	// 移動限界座標
	const float kMoveLimitX = 1.9f;
	const float kMoveLimitY = 1.0f;

	// 範囲超えない処理
	transform.translate.x = max(transform.translate.x, -kMoveLimitX);
	transform.translate.x = min(transform.translate.x, +kMoveLimitX);
	transform.translate.y = max(transform.translate.y, -kMoveLimitY);
	transform.translate.y = min(transform.translate.y, +kMoveLimitY);

	Attack();

	// 弾更新
	for (PlayerBullet* bullet : bullets_) {
		bullet->Update();
	}

	ImGui::Begin("Player");
	ImGui::SliderFloat3("translate", &transform.translate.x, -10.0f, 10.0f);
	ImGui::End();

}

void Player::Draw(uint32_t index, Camera* camera){
	model_->Draw(camera, index);

	// 弾描画
	for (PlayerBullet* bullet : bullets_) {
		bullet->Draw(camera, bulletTex);
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