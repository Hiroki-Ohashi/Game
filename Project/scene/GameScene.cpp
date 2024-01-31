#include "GameScene.h"

GameScene::~GameScene(){
	delete camera_;
	delete title_;
	delete kakusi_;
	delete ui_;
	delete skydome_;
}

void GameScene::Initialize(){
	textureManager_ = TextureManager::GetInstance();
	textureManager_->Initialize();

	input_ = Input::GetInsTance();

	camera_ = new Camera();
	camera_->Initialize();

	player_ = new Player();
	player_->Init();

	enemy_ = new Enemy();
	enemy_->Init();
	// 敵キャラに自キャラのアドレスを渡す
	enemy_->SetPlayer(player_);

	title_ = new Sprite();
	title_->Initialize({0,0}, {1280, 720}, 1.0f);

	kakusi_ = new Sprite();
	kakusi_->Initialize({ 0,0 }, { 1280, 720 }, 0.0f);

	ui_ = new Sprite();
	ui_->Initialize({ 460,360 }, { 360, 180 }, 0.01f);

	// 天球の生成
	skydome_ = new Skydome();
	// 天球の初期化
	skydome_->Initialize();

	uv = textureManager_->Load("Resources/start.png");
	kuro = textureManager_->Load("Resources/kuro.png");
	teki= textureManager_->Load("Resources/moon.png");
	ui = textureManager_->Load("Resources/ui.png");
	sky = textureManager_->Load("Resources/sky.png");

	a = 0.0f;
	isAAA = false;
	isTitle = true;

	scene = 0;
}

void GameScene::Update(){
	camera_->Update();


	if (scene == 0) {
		if (input_->TriggerKey(DIK_SPACE)) {
			isAAA = true;
		}

		if (isAAA) {
			a += speed;
		}

		if (a >= 1.2f) {
			speed *= -1;
			isTitle = false;
		}

		b += speed2;

		if (b >= 1.0f) {
			speed2 *= -1;
		}
		if (b <= 0.0f) {
			speed2 *= -1;
		}

		if (a <= -0.01f) {
			scene = 1;
		}
	}

	kakusi_->Update(a);
	ui_->Update(b);


	if (scene == 1) {
		player_->Update();

		// 敵キャラの更新
		if (enemy_ != nullptr) {
			enemy_->Update();
		}

		camera_->cameraTransform.translate.z -= 0.05f;
		CheckAllCollisions();
	}
}

void GameScene::Draw(){
	player_->Draw(kuro, camera_, kuro);
	enemy_->Draw(teki, camera_, kuro);
	skydome_->Draw(camera_, sky);

	if (isTitle) {
		title_->Draw(uv);
		ui_->Draw(ui);
	}
	
	kakusi_->Draw(kuro);
}

void GameScene::Release() {
}

void GameScene::CheckAllCollisions()
{
	// 判定衝突AとBの座標
	Vector3 posA, posB;

	// 自弾リストの取得
	const std::list<PlayerBullet*>& playerBullets = player_->GetBullets();
	// 敵弾リストの取得
	const std::list<EnemyBullet*>& enemyBullets = enemy_->GetBullets();

#pragma region 自キャラと敵弾の当たり判定
	// 自キャラの座標
	posA = player_->GetPosition();

	// 自キャラと敵弾すべての当たり判定
	for (EnemyBullet* bullet : enemyBullets) {
		// 敵弾の座標
		posB = bullet->GetPosition();

		float p2eBX = (posB.x - posA.x) * (posB.x - posA.x);
		float p2eBY = (posB.y - posA.y) * (posB.y - posA.y);
		float p2eBZ = (posB.z - posA.z) * (posB.z - posA.z);

		float pRadius = 1;
		float eBRadius = 1;

		float L = (pRadius + eBRadius) * (pRadius + eBRadius);

		if (p2eBX + p2eBY + p2eBZ <= L) {
			// 自キャラの衝突時コールバックを呼び出す
			player_->OnCollision();
			// 敵弾の衝突時コールバックを呼び出す
			bullet->OnCollision();
		}

	}

#pragma endregion

#pragma region 自弾と敵キャラの当たり判定
	// 敵キャラの座標
	posA = enemy_->GetPosition();

	// 敵キャラと自弾すべての当たり判定
	for (PlayerBullet* bullet : playerBullets) {
		// 自弾の座標
		posB = bullet->GetPosition();

		float e2pBX = (posB.x - posA.x) * (posB.x - posA.x);
		float e2pBY = (posB.y - posA.y) * (posB.y - posA.y);
		float e2pBZ = (posB.z - posA.z) * (posB.z - posA.z);

		const int eRadius = 1;
		const int pBRadius = 1;

		float L = (eRadius + pBRadius);

		if (e2pBX + e2pBY + e2pBZ <= L) {
			// 敵キャラの衝突時コールバックを呼び出す
			enemy_->OnCollision();
			// 自弾の衝突時コールバックを呼び出す
			bullet->OnCollision();
		}
	}
#pragma endregion

#pragma region 自弾と敵弾の当たり判定
	// 敵弾と自弾すべての当たり判定
	for (PlayerBullet* playerBullet : playerBullets) {
		for (EnemyBullet* enemyBullet : enemyBullets) {
			// 敵弾の座標
			posA = playerBullet->GetPosition();
			// 自弾の座標
			posB = enemyBullet->GetPosition();

			float pB2eBX = (posB.x - posA.x) * (posB.x - posA.x);
			float pB2eBY = (posB.y - posA.y) * (posB.y - posA.y);
			float pB2eBZ = (posB.z - posA.z) * (posB.z - posA.z);

			float pBRadius = 1;
			float eBRadius = 1;

			float L = (pBRadius + eBRadius) * (pBRadius + eBRadius);

			if (pB2eBX + pB2eBY + pB2eBZ <= L) {
				// 自弾の衝突時コールバックを呼び出す
				playerBullet->OnCollision();
				// 敵弾の衝突時コールバックを呼び出す
				enemyBullet->OnCollision();
			}
		}
	}
#pragma endregion
}
