#include "GameScene.h"

GameScene::~GameScene(){
	delete camera_;

	// bullet_の解放
	for (EnemyBullet* bullet : enemyBullets_) {
		delete bullet;
	}

	for (BossBullet* bullet : bossBullets_) {
		delete bullet;
	}

	for (Enemy* enemy : enemys_) {
		delete enemy;
	}
}

void GameScene::Initialize() {
	camera_ = new Camera();
	camera_->Initialize();

	postProcess_ = std::make_unique<PostProcess>();
	postProcess_->Initialize();

	// player
	player_ = std::make_unique<Player>();
	player_->Initialize();

	// boss
	boss_ = std::make_unique<Boss>();
	boss_->Initialize(pos2_);
	boss_->SetPlayer(player_.get());
	boss_->SetGameScene(this);

	// stage
	stage_ = std::make_unique<Stage>();
	stage_->Initialize();

	// skybox
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();

	enemyBulletTex = textureManager_->Load("resources/black.png");
	bossBulletTex = textureManager_->Load("resources/white.png");

	for (Enemy* enemy : enemys_) {
		enemy->SetIsDead(false);
	}

	LoadEnemyPopData();
}

void GameScene::Update(){

	UpdateEnemyPopCommands();

	camera_->Update();

	player_->Update();

	if (player_->GetPos().z >= 500.0f) {

		boss_->Update();

		// ボス弾更新
		for (BossBullet* bullet : bossBullets_) {
			bullet->Update();
		}

		bossBullets_.remove_if([](BossBullet* bullet) {
			if (bullet->IsDead()) {
				delete bullet;
				return true;
			}
			return false;
			});
	}

	for (Enemy* enemy : enemys_) {
		enemy->Update();
	}

	// 弾更新
	for (EnemyBullet* bullet : enemyBullets_) {
		bullet->Update();
	}

	// デスフラグの立った弾を排除
	enemyBullets_.remove_if([](EnemyBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

	CheckAllCollisions();

	stage_->Update();

	camera_->cameraTransform.translate = { player_->GetPos().x, player_->GetPos().y + 3.0f,  player_->GetPos().z - 70.0f };
	//camera_->cameraTransform.translate = { 0.0f, 0.0f,  player_->GetPos().z - 50.0f };

	if (boss_->IsDead() == true) {
		sceneNo = CLEAR;
	}
}

void GameScene::Draw()
{
	skydome_->Draw(camera_);

	stage_->Draw(camera_);

	// 敵キャラの描画
	for (Enemy* enemy : enemys_) {
		enemy->Draw(camera_);
	}

	// 弾描画
	for (EnemyBullet* bullet : enemyBullets_) {
		bullet->Draw(camera_, enemyBulletTex);
	}

	player_->Draw(camera_);

	if (player_->GetPos().z >= 500.0f) {

		boss_->Draw(camera_);

		// ボス弾描画
		for (BossBullet* bullet : bossBullets_) {
			bullet->Draw(camera_, bossBulletTex);
		}
	}
}


void GameScene::PostDraw()
{
	postProcess_->Draw();
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
	const std::list<BossBullet*>& bossBullets = bossBullets_;

#pragma region 自キャラと敵弾の当たり判定
	// 自キャラの座標
	posA = player_->GetPos();

	// 自キャラと敵弾すべての当たり判定
	for (EnemyBullet* bullet : enemyBullets) {
		// 敵弾の座標
		posB = bullet->GetPos();

		float p2eBX = (posB.x - posA.x) * (posB.x - posA.x);
		float p2eBY = (posB.y - posA.y) * (posB.y - posA.y);
		float p2eBZ = (posB.z - posA.z) * (posB.z - posA.z);

		float pRadius = 1.0f;
		float eBRadius = 1.0f;

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
			posA = enemy->GetPos();

			// 自弾の座標
			posB = bullet->GetPos();

			float e2pBX = (posB.x - posA.x) * (posB.x - posA.x);
			float e2pBY = (posB.y - posA.y) * (posB.y - posA.y);
			float e2pBZ = (posB.z - posA.z) * (posB.z - posA.z);

			float eRadius = 1.0f;
			float pBRadius = 1.0f;

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
			posA = playerBullet->GetPos();
			// 自弾の座標
			posB = enemyBullet->GetPos();

			float pB2eBX = (posB.x - posA.x) * (posB.x - posA.x);
			float pB2eBY = (posB.y - posA.y) * (posB.y - posA.y);
			float pB2eBZ = (posB.z - posA.z) * (posB.z - posA.z);

			float pBRadius = 1.0f;
			float eBRadius = 1.0f;

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

#pragma region 自弾とボスの当たり判定
	// 自キャラの座標
	posA = boss_->GetPos();

	// 自キャラと敵弾すべての当たり判定
	for (PlayerBullet* playerBullet : playerBullets) {
		// 敵弾の座標
		posB = playerBullet->GetPos();

		float p2eBX = (posB.x - posA.x) * (posB.x - posA.x);
		float p2eBY = (posB.y - posA.y) * (posB.y - posA.y);
		float p2eBZ = (posB.z - posA.z) * (posB.z - posA.z);

		float pRadius = 15.0f;
		float eBRadius = 1.0f;

		float L = (pRadius + eBRadius) * (pRadius + eBRadius);

		if (p2eBX + p2eBY + p2eBZ <= L) {
			// 自キャラの衝突時コールバックを呼び出す
			if (player_->GetPos().z >= 500.0f) {
				boss_->OnCollision();
			}
			// 敵弾の衝突時コールバックを呼び出す
			playerBullet->OnCollision();
		}
	}
#pragma endregion

#pragma region 自キャラとボス弾の当たり判定
	// 自キャラの座標
	posA = player_->GetPos();

	// 自キャラと敵弾すべての当たり判定
	for (BossBullet* bullet : bossBullets) {
		// 敵弾の座標
		posB = bullet->GetPos();

		float p2eBX = (posB.x - posA.x) * (posB.x - posA.x);
		float p2eBY = (posB.y - posA.y) * (posB.y - posA.y);
		float p2eBZ = (posB.z - posA.z) * (posB.z - posA.z);

		float pRadius = 1.0f;
		float eBRadius = 1.0f;

		float L = (pRadius + eBRadius) * (pRadius + eBRadius);

		if (p2eBX + p2eBY + p2eBZ <= L) {
			// 自キャラの衝突時コールバックを呼び出す
			player_->OnCollision();
			// 敵弾の衝突時コールバックを呼び出す
			bullet->OnCollision();
		}
	}

#pragma endregion

}

void GameScene::LoadEnemyPopData()
{
	// ファイルを開く
	std::ifstream file;
	file.open("Resources/enemyPop.csv");
	assert(file.is_open());

	// ファイルも内容を文字列ストリームにコピー
	enemyPopCommands << file.rdbuf();

	// ファイルを閉じる
	file.close();
}

void GameScene::UpdateEnemyPopCommands()
{
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

void GameScene::EnemySpown(Vector3 pos)
{
	// 敵キャラの生成
	Enemy* enemy_ = new Enemy();
	// 敵キャラの初期化
	enemy_->Initialize(pos);
	// 敵キャラに自キャラのアドレスを渡す
	enemy_->SetPlayer(player_.get());
	// 敵キャラにゲームシーンを渡す
	enemy_->SetGameScene(this);
	AddEnemy(enemy_);
}

void GameScene::AddEnemyBullet(EnemyBullet* enemyBullet)
{
	// リストに登録する
	enemyBullets_.push_back(enemyBullet);
}

void GameScene::AddBossBullet(BossBullet* bossBullet)
{
	// リストに登録する
	bossBullets_.push_back(bossBullet);
}

void GameScene::AddEnemy(Enemy* enemy)
{
	// リストに登録する
	enemys_.push_back(enemy);
}
