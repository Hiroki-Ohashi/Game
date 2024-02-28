#include "Enemy.h"
#include "Player.h"
#include "GameScene.h"

Enemy::~Enemy() {
}

void Enemy::Init(Vector3 translation) {
	transform = { { 0.1f,0.1f,0.1f},{0.0f,0.0f,0.0f},{translation.x,translation.y,translation.z} };

	model_ = std::make_unique<Model>();
	model_->Initialize("cube.obj", transform);

	isDead_ = false;
}

// staticで宣言したメンバ関数ポインタテーブルの実態
void (Enemy::* Enemy::phasePFuncTable[])() = { &Enemy::ApproachUpdate, &Enemy::LeaveUpdate };

void Enemy::Update() {

	// メンバ関数ポインタに入っている関数を呼び出す
	(this->*phasePFuncTable[static_cast<size_t>(phase_)])();

	model_->worldTransform_.translate = transform.translate;

	attackTimer--;

	if (attackTimer <= 0) {

		if (isDead_ == false) {
			// 攻撃処理
			Attack();
		}

		// 発射タイマーを初期化
		attackTimer = kFireInterval;
	}
}

void Enemy::Draw(uint32_t index, Camera* camera) {
	if (isDead_ == false) {
		model_->Draw(camera, index);
	}
}

void Enemy::Attack() {
	// 弾の速度
	// 弾の速度
	const float kBulletSpeed = 0.01f;


	Vector3 end = player_->GetPosition();
	Vector3 start = transform.translate;
	Vector3 diff;

	diff.x = end.x - start.x;
	diff.y = end.y - start.y;
	diff.z = end.z + start.z;

	diff = Normalize(diff);

	diff.x *= kBulletSpeed;
	diff.y *= kBulletSpeed;
	diff.z *= kBulletSpeed;

	Vector3 velocity(diff.x, diff.y, diff.z);


	// 弾を生成し、初期化
	EnemyBullet* newBullet = new EnemyBullet();
	newBullet->SetPlayer(player_);
	newBullet->Init(transform.translate, velocity);
	// 弾を登録
	gameScene_->AddEnemyBullet(newBullet);
}

void Enemy::ApproachUpdate() {
	// 移動 (ベクトルを加算)
	transform.translate.z -= 0.05f;
	// 既定の位置に達したら離脱
	if (transform.translate.z < 0.0f) {
		phase_ = Phase::Leave;
	}
}

void Enemy::LeaveUpdate() {
	// 移動 (ベクトルを加算)
	if (phase_ == Phase::Leave) {
		transform.translate.y += 0.01f;
		transform.translate.x -= 0.01f;
	}
}

void  Enemy::SetPlayer(Player* player) { player_ = player; }