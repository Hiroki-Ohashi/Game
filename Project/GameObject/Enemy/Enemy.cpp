#include "Enemy.h"
#include "Player.h"

Enemy::~Enemy() {
	delete model_;
}

void Enemy::Init() {
	transform = { { 0.1f,0.1f,0.1f},{0.0f,0.0f,0.0f},{0.01f,0.5f,10.0f} };

	model_ = new Model();
	model_->Initialize("cube.obj", transform);
}

// staticで宣言したメンバ関数ポインタテーブルの実態
void (Enemy::* Enemy::phasePFuncTable[])() = { &Enemy::ApproachUpdate, &Enemy::LeaveUpdate };

void Enemy::Update() {

	// デスフラグんお立った弾を排除
	bullets_.remove_if([](EnemyBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

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

	// 弾更新
	for (EnemyBullet* bullet : bullets_) {
		bullet->Update();
	}
}

void Enemy::Draw(uint32_t index, Camera* camera, uint32_t index2) {
	if (isDead_ == false) {
		model_->Draw(camera, index);
	}

	// 弾描画
	for (EnemyBullet* bullet : bullets_) {
		bullet->Draw(camera, index2);
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
	bullets_.push_back(newBullet);
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