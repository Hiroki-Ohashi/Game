#include "GameScene.h"

GameScene::~GameScene(){
	delete camera_;
	delete title_;
	delete kakusi_;
	delete ui_;
	delete player_;

	// bullet_の解放
	for (EnemyBullet* bullet : enemyBullets_) {
		delete bullet;
	}

	for (Enemy* enemy : enemys_) {
		delete enemy;
	}
}

void GameScene::Initialize(){
	textureManager_ = TextureManager::GetInstance();
	textureManager_->Initialize();

	input_ = Input::GetInsTance();

	camera_ = new Camera();
	camera_->Initialize();

	player_ = new Player;
	player_->Init();

	LoadEnemyPopData();

	title_ = new Sprite();
	title_->Initialize({0,0}, {1280, 720}, 1.0f);

	kakusi_ = new Sprite();
	kakusi_->Initialize({ 0,0 }, { 1280, 720 }, 0.0f);

	ui_ = new Sprite();
	ui_->Initialize({ 460,360 }, { 360, 180 }, 0.01f);

	// 天球の生成
	skydome_ = std::make_unique<Skydome>();
	// 天球の初期化
	skydome_->Initialize();

	uv = textureManager_->Load("resources/start.png");
	kuro = textureManager_->Load("resources/kuro.png");
	teki= textureManager_->Load("resources/moon.png");
	ui = textureManager_->Load("resources/ui.png");
	sky = textureManager_->Load("resources/sky.png");
	reticle = textureManager_->Load("resources/reticle.png");

	a = 0.0f;
	isAAA = false;
	isTitle = true;

	scene = 0;
}

void GameScene::Update(){
	camera_->Update();


	if (scene == 0) {

		XINPUT_STATE joyState;

		if (Input::GetInsTance()->GetJoystickState(joyState)) {

			if (Input::GetInsTance()->PressedButton(joyState, XINPUT_GAMEPAD_A)) {
				isAAA = true;
			}

		}


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

		UpdateEnemyPopCommands();

		for (Enemy* enemy : enemys_) {
			enemy->Update();
		}

		// 弾更新
		for (EnemyBullet* bullet : enemyBullets_) {
			bullet->Update();
		}

		// デスフラグんお立った弾を排除
		enemyBullets_.remove_if([](EnemyBullet* bullet) {
			if (bullet->IsDead()) {
				delete bullet;
				return true;
			}
			return false;
		});

		camera_->cameraTransform.translate.z += 0.001f;
		CheckAllCollisions();
	}
}

void GameScene::Draw(){
	player_->Draw(kuro, camera_, kuro);
	// 敵キャラの描画
	for (Enemy* enemy : enemys_) {
		enemy->Draw(teki, camera_);
	}

	// 弾描画
	for (EnemyBullet* bullet : enemyBullets_) {
		bullet->Draw(camera_, kuro);
	}
	skydome_->Draw(camera_, sky);

	player_->DrawUI(reticle);

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
	const std::list<EnemyBullet*>& enemyBullets = enemyBullets_;

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

		float pRadius = 0.1f;
		float eBRadius = 0.1f;

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
	// 敵キャラと自弾すべての当たり判定
	for (PlayerBullet* bullet : playerBullets) {
		for (Enemy* enemy : enemys_) {

			// 敵キャラの座標
			posA = enemy->GetPosition();

			// 自弾の座標
			posB = bullet->GetPosition();

			float e2pBX = (posB.x - posA.x) * (posB.x - posA.x);
			float e2pBY = (posB.y - posA.y) * (posB.y - posA.y);
			float e2pBZ = (posB.z - posA.z) * (posB.z - posA.z);

			float eRadius = 0.1f;
			float pBRadius = 0.1f;

			float L = (eRadius + pBRadius);

			if (e2pBX + e2pBY + e2pBZ <= L) {
				// 敵キャラの衝突時コールバックを呼び出す
				enemy->OnCollision();
				// 自弾の衝突時コールバックを呼び出す
				bullet->OnCollision();
			}
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

			float pBRadius = 0.1f;
			float eBRadius = 0.1f;

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

void GameScene::LoadEnemyPopData() {
	// ファイルを開く
	std::ifstream file;
	file.open("Resources/enemyPop.csv");
	assert(file.is_open());

	// ファイルも内容を文字列ストリームにコピー
	enemyPopCommands << file.rdbuf();

	// ファイルを閉じる
	file.close();
}

void GameScene::UpdateEnemyPopCommands() {

	// 待機処理
	if (isWait_) {
		waitTimer_--;
		if (waitTimer_ <= 0) {
			// 待機完了
			isWait_ = false;
		}
		return;
	}

	// 1行分の文字列を入れる変数
	std::string line;

	// コマンド実行ループ
	while (getline(enemyPopCommands, line)) {
		// 1行分の文字列をストリームに変換して解析しやすくなる
		std::istringstream line_stream(line);

		std::string word;
		// ,区切りで行の先頭文字列を取得
		getline(line_stream, word, ',');

		// "//"から始まる行はコメント
		if (word.find("//") == 0) {
			// コメント行を飛ばす
			continue;
		}

		// POPコマンド
		if (word.find("POP") == 0) {
			// X座標
			getline(line_stream, word, ',');
			float x = (float)std::atof(word.c_str());

			// Y座標
			getline(line_stream, word, ',');
			float y = (float)std::atof(word.c_str());

			// Z座標
			getline(line_stream, word, ',');
			float z = (float)std::atof(word.c_str());

			// 敵を発生させる
			EnemySpown(Vector3(x, y, z));
		}

		// WAITコマンド
		else if (word.find("WAIT") == 0) {
			getline(line_stream, word, ',');

			// 待ち時間
			int32_t waitTime = atoi(word.c_str());

			// 待機開始
			isWait_ = true;
			waitTimer_ = waitTime;

			// コマンドループを抜ける
			break;
		}
	}
}

void GameScene::EnemySpown(Vector3 translation) {
	// 敵キャラの生成
	Enemy* enemy_ = new Enemy();
	// 敵キャラの初期化
	enemy_->Init(translation);
	// 敵キャラにゲームシーンを渡す
	enemy_->SetGameScene(this);
	// 敵キャラに自キャラのアドレスを渡す
	enemy_->SetPlayer(player_);
	AddEnemy(enemy_);
}

void GameScene::AddEnemy(Enemy* enemy) {
	// リストに登録する
	enemys_.push_back(enemy);
}

void GameScene::AddEnemyBullet(EnemyBullet* enemyBullet) {
	// リストに登録する
	enemyBullets_.push_back(enemyBullet);
}
