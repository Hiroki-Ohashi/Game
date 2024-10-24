#include "GameScene.h"
#include <time.h>

/// <summary>
/// GameScene.cpp
/// ゲームシーンのソースファイル
/// </summary>

GameScene::~GameScene(){
}

void GameScene::Initialize() {
	camera_.Initialize();

	postProcess_ = std::make_unique<PostProcess>();
	postProcess_->Initialize(NOISE);
	postProcess_->SetVignette(0.0f, 16.0f);
	postProcess_->SetNoise(0.0f, 0.0f);
	postProcess_->SetBlurStrength(0.0f);

	// player
	player_ = std::make_unique<Player>();
	player_->Initialize();

	// boss
	boss_ = std::make_unique<Boss>();
	boss_->Initialize(pos2_);
	boss_->SetPlayer(player_.get());
	boss_->SetGameScene(this);

	// ready
	ready_ = std::make_unique<Sprite>();
	ready_->Initialize(Vector2{ 200.0f, 25.0f }, Vector2{ 450.0, 150.0f }, 1.0f);

	transform_ = { {10.0f,10.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,30.0f,-100.0f} };
	go_ = std::make_unique<Model>();
	go_->Initialize("board.obj", transform_);
	go_->SetLight(false);

	// skybox
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();

	enemyBulletTex = textureManager_->Load("resources/black.png");
	bossBulletTex = textureManager_->Load("resources/white.png");
	uv = textureManager_->Load("resources/map.png");
	ready = textureManager_->Load("resources/ready.png");
	go = textureManager_->Load("resources/go.png");

	for (std::unique_ptr<Enemy>& enemy : enemys_) {
		enemy->SetIsDead(false);
	}

	LoadEnemyPopData();
  
    json_ = std::make_unique<Json>();
	levelData_ = json_->LoadJson("level");
	json_->Adoption(levelData_, false);

	isVignette_ = true;
	isNoise_ = false;
	isApploach_ = true;

	camera_.cameraTransform.translate = { player_->GetPos().x, player_->GetPos().y + 3.0f,  player_->GetPos().z - 30.0f };
	blurStrength_ = 0.3f;
}

void GameScene::Update(){

	UpdateEnemyPopCommands();

	camera_.Update();

	postProcess_->NoiseUpdate(0.1f);

	if (isVignette_) {
		postProcess_->VignetteFadeOut(0.1f, 0.1f, 16.0f, 0.0f);
	}

	if (postProcess_->GetVignetteShape() <= 0.0f) {
		isVignette_ = false;
	}

	if (isVignette_ == false) {
		if (isApploach_ == false) {
			player_->Update();
		}
	}

	if (player_->GetPos().z >= 500.0f) {

		boss_->Update();

		// ボス弾更新
		for (std::unique_ptr<BossBullet>& bullet : bossBullets_) {
			bullet->Update();
		}

		bossBullets_.erase(
			std::remove_if(
				bossBullets_.begin(),
				bossBullets_.end(),
				[](const std::unique_ptr<BossBullet>& bullet) {
					return bullet->IsDead();
				}
			),
			bossBullets_.end()
		);
	}

	json_->Update();
  
	for (std::unique_ptr<Enemy>& enemy : enemys_) {
		enemy->Update();
	}

	// 弾更新
	for (std::unique_ptr<EnemyBullet>& bullet : enemyBullets_) {
		bullet->Update();
	}

	// デスフラグの立った弾を排除
	enemyBullets_.erase(
		std::remove_if(
			enemyBullets_.begin(),
			enemyBullets_.end(),
			[](const std::unique_ptr<EnemyBullet>& bullet) {
				return bullet->IsDead();
			}
		),
		enemyBullets_.end()
	);

	CheckAllCollisions();

	if (isShake) {
		shakeTimer -= 1;
		if (shakeTimer >= 30) {
			randX = rand() % 2 - 1;
			randY = rand() % 2 - 1;
		}
		if (shakeTimer <= 0) {
			isShake = false;
		}
	}
	else 
	{
		randX = 0;
		randY = 0;
	}

	if (isVignette_ == false) {
		if (isApploach_) {
			camera_.cameraTransform.rotate.y += 0.035f;

			EulerTransform origin = { {0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f},{player_->GetPos().x,player_->GetPos().y,player_->GetPos().z} };
			// 追従対象からカメラまでのオフセット
			Vector3 offset = { 0.0f, 3.0f, -30.0f };
			// カメラの角度から回転行列を計算する
			Matrix4x4 worldTransform = MakeRotateYMatrix(camera_.cameraTransform.rotate.y);
			// オフセットをカメラの回転に合わせて回転させる
			offset = TransformNormal(offset, worldTransform);
			// 座標をコピーしてオフセット分ずらす
			camera_.cameraTransform.translate.x = origin.translate.x + offset.x;
			camera_.cameraTransform.translate.y = origin.translate.y + offset.y;
			camera_.cameraTransform.translate.z = origin.translate.z + offset.z;

			time_ += 1;

			if (time_ >= 180) {
				time_ = 0;
				isApploach_ = false;
				postProcess_->SetBlurStrength(blurStrength_);
			}
		}
		else if (isApploach_ == false) {
			camera_.cameraTransform.translate = { player_->GetPos().x + randX, player_->GetPos().y + 3.0f + randY,  player_->GetPos().z - 30.0f };

			blurStrength_ -= 0.002f;
			if (blurStrength_ <= 0.0f) {
				blurStrength_ = 0.0f;
			}

			postProcess_->SetBlurStrength(blurStrength_);
		}
	}

	if (boss_->IsDead() == true) {
		isNoise_ = true;
	}

	if (isNoise_) {
		if (noiseStrength <= 100.0f) {
			noiseStrength += 1.0f;
		}
		
		if(postProcess_->GetNoiseStrength() >= 100.0f){
			noiseStrength = 0.0f;
			sceneNo = CLEAR;
		}
	}

	postProcess_->SetNoiseStrength(noiseStrength);
}

void GameScene::Draw()
{

	skydome_->Draw(&camera_);

	json_->Draw(camera_, uv);

	player_->BulletDraw(&camera_);

	// 敵キャラの描画
	for (std::unique_ptr<Enemy>& enemy : enemys_) {
		enemy->Draw(&camera_);
	}

	// 弾描画
	for (std::unique_ptr<EnemyBullet>& bullet : enemyBullets_) {
		bullet->Draw(&camera_, enemyBulletTex);
	}

	if (player_->GetPos().z >= 500.0f) {

		boss_->Draw(&camera_);

		// ボス弾描画
		for (std::unique_ptr<BossBullet>& bullet : bossBullets_) {
			bullet->Draw(&camera_, bossBulletTex);
		}
	}

	if (isApploach_) {
		ready_->Draw(ready);
	}
	else {
		go_->Draw(&camera_, go);
	}

	player_->Draw(&camera_);
}


void GameScene::PostDraw()
{
	postProcess_->NoiseDraw();
}

void GameScene::Release() {
}

void GameScene::CheckAllCollisions()
{
	// 判定衝突AとBの座標
	Vector3 posA, posB;

	// 自弾リストの取得
	std::vector<std::unique_ptr<PlayerBullet>>& playerBullets = player_->GetBullets();
	// 敵弾リストの取得
	std::vector<std::unique_ptr<EnemyBullet>>& enemyBullets = enemyBullets_;
	std::vector<std::unique_ptr<BossBullet>>& bossBullets = bossBullets_;

#pragma region 自キャラと敵弾の当たり判定
	// 自キャラの座標
	posA = player_->GetPos();

	// 自キャラと敵弾すべての当たり判定
	for (std::unique_ptr<EnemyBullet>& bullet : enemyBullets) {

		if (bullet->IsDead()) {
			continue; // 既に削除された弾はスキップ
		}

		// 敵弾の座標
		posB = bullet->GetPos();

		float p2eBX = (posB.x - posA.x) * (posB.x - posA.x);
		float p2eBY = (posB.y - posA.y) * (posB.y - posA.y);
		float p2eBZ = (posB.z - posA.z) * (posB.z - posA.z);

		float pRadius = 1.0f;
		float eBRadius = 1.0f;

		float L = (pRadius + eBRadius) * (pRadius + eBRadius);

		if (p2eBX + p2eBY + p2eBZ <= L) {

			shakeTimer = 40;
			isShake = true;

			// 自キャラの衝突時コールバックを呼び出す
			player_->OnCollision();
			// 敵弾の衝突時コールバックを呼び出す
			bullet->OnCollision();
		}
	}

#pragma endregion

#pragma region 自弾と敵キャラの当たり判定
	// 敵キャラと自弾すべての当たり判定
	for (std::unique_ptr<PlayerBullet>& bullet : playerBullets) {

		if (bullet->IsDead()) {
			continue; // 既に削除された弾はスキップ
		}

		for (std::unique_ptr<Enemy>& enemy : enemys_) {

			if (enemy->IsDead()) {
				continue; // 既に削除された敵はスキップ
			}

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
	for (std::unique_ptr<PlayerBullet>& playerBullet : playerBullets) {

		if (playerBullet->IsDead()) {
			continue; // 既に削除された弾はスキップ
		}

		for (std::unique_ptr<EnemyBullet>& enemyBullet : enemyBullets) {

			if (enemyBullet->IsDead()) {
				continue; // 既に削除された弾はスキップ
			}

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
	for (std::unique_ptr<PlayerBullet>& playerBullet : playerBullets) {

		if (playerBullet->IsDead()) {
			continue; // 既に削除された弾はスキップ
		}

		// 敵弾の座標
		posB = playerBullet->GetPos();

		float p2eBX = (posB.x - posA.x) * (posB.x - posA.x);
		float p2eBY = (posB.y - posA.y) * (posB.y - posA.y);
		float p2eBZ = (posB.z - posA.z) * (posB.z - posA.z);
		float pRadius = 30.0f;
		float eBRadius = 5.0f;

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
	for (std::unique_ptr<BossBullet>& bullet : bossBullets) {

		if (bullet->IsDead()) {
			continue; // 既に削除された弾はスキップ
		}

		// 敵弾の座標
		posB = bullet->GetPos();

		float p2eBX = (posB.x - posA.x) * (posB.x - posA.x);
		float p2eBY = (posB.y - posA.y) * (posB.y - posA.y);
		float p2eBZ = (posB.z - posA.z) * (posB.z - posA.z);

		float pRadius = 1.0f;
		float eBRadius = 1.0f;

		float L = (pRadius + eBRadius) * (pRadius + eBRadius);

		if (p2eBX + p2eBY + p2eBZ <= L) {

			shakeTimer = 40;
			isShake = true;

			// 自キャラの衝突時コールバックを呼び出す
			player_->OnCollision();
			// 敵弾の衝突時コールバックを呼び出す
			bullet->OnCollision();
		}
	}

#pragma endregion

#pragma region 自弾とボス弾の当たり判定
	// 敵弾と自弾すべての当たり判定
	for (std::unique_ptr<PlayerBullet>& playerBullet : playerBullets) {

		if (playerBullet->IsDead()) {
			continue; // 既に削除された弾はスキップ
		}

		for (std::unique_ptr<BossBullet>& bossBullet : bossBullets) {

			if (bossBullet->IsDead()) {
				continue; // 既に削除された弾はスキップ
			}

			// 敵弾の座標
			posA = playerBullet->GetPos();
			// 自弾の座標
			posB = bossBullet->GetPos();

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
				bossBullet->OnCollision();
			}
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
	std::unique_ptr<Enemy> enemy_ = std::make_unique<Enemy>();
	// 敵キャラの初期化
	enemy_->Initialize(pos);
	// 敵キャラに自キャラのアドレスを渡す
	enemy_->SetPlayer(player_.get());
	// 敵キャラにゲームシーンを渡す
	enemy_->SetGameScene(this);
	AddEnemy(std::move(enemy_));
}

void GameScene::AddEnemyBullet(std::unique_ptr<EnemyBullet> enemyBullet)
{
	// リストに登録する
	enemyBullets_.push_back(std::move(enemyBullet));
}

void GameScene::AddBossBullet(std::unique_ptr<BossBullet> bossBullet)
{
	// リストに登録する
	bossBullets_.push_back(std::move(bossBullet));
}

void GameScene::AddEnemy(std::unique_ptr<Enemy> enemy)
{
	// リストに登録する
	enemys_.push_back(std::move(enemy));
}
