#include "GameScene.h"

GameScene::~GameScene(){
	delete camera_;

	// bullet_の解放
	for (EnemyBullet* bullet : enemyBullets_) {
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

	// stage
	stage_ = std::make_unique<Stage>();
	stage_->Initialize();

	// skybox
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();

	enemyBulletTex = textureManager_->Load("resources/black.png");

	EnemySpown(pos_);

}

void GameScene::Update(){

	camera_->Update();
	player_->Update();

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

	stage_->Update();

	camera_->cameraTransform.translate.z = player_->GetPos().z - 50.0f;

	if (input_->TriggerKey(DIK_RETURN)) {
		sceneNo = CLEAR;
	}
}

void GameScene::Draw()
{
	skydome_->Draw(camera_);

	player_->Draw(camera_);

	// 敵キャラの描画
	for (Enemy* enemy : enemys_) {
		enemy->Draw(camera_);
	}

	// 弾描画
	for (EnemyBullet* bullet : enemyBullets_) {
		bullet->Draw(camera_, enemyBulletTex);
	}

	stage_->Draw(camera_);
}


void GameScene::PostDraw()
{
	postProcess_->Draw();
}

void GameScene::Release() {
}

void GameScene::EnemySpown(Vector3 pos)
{
	// 敵キャラの生成
	Enemy* enemy_ = new Enemy();
	// 敵キャラの初期化
	enemy_->Initialize(pos);
	// 敵キャラにゲームシーンを渡す
	enemy_->SetGameScene(this);
	// 敵キャラに自キャラのアドレスを渡す
	enemy_->SetPlayer(player_.get());
	AddEnemy(enemy_);
}

void GameScene::AddEnemyBullet(EnemyBullet* enemyBullet)
{
	// リストに登録する
	enemyBullets_.push_back(enemyBullet);
}

void GameScene::AddEnemy(Enemy* enemy)
{
	// リストに登録する
	enemys_.push_back(enemy);
}
