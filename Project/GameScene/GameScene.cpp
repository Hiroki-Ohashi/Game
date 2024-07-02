#include "GameScene.h"

GameScene::~GameScene(){
	delete camera_;
}

void GameScene::Initialize(){
	textureManager_ = TextureManager::GetInstance();
	textureManager_->Initialize();

	camera_ = new Camera();
	camera_->Initialize();

	postProcess_ = std::make_unique<PostProcess>();
	postProcess_->Initialize();

	// player
	player_ = std::make_unique<Player>();
	player_->Initialize();

	// skybox
	skyBox_ = std::make_unique<SkyBox>();
	skyBox_->Initialize();

	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();

	// texture
	playerTex = textureManager_->Load("resources/white.png");
	skybox = textureManager_->Load("resources/rostock_laage_airport_4k.dds");
}

void GameScene::Update(){

	camera_->Update();
	player_->Update();
}

void GameScene::Draw()
{
	skydome_->Draw(camera_);
	player_->Draw(camera_, playerTex);
	//skyBox_->Draw(camera_, skybox);
}


void GameScene::PostDraw()
{
	postProcess_->Draw();
}

void GameScene::Release() {
}
