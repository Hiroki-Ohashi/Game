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

	player_ = std::make_unique<Player>();
	player_->Init();

	playerTex = textureManager_->Load("resources/uvChecker.png");
}

void GameScene::Update(){

	camera_->Update();
	player_->Update();
}

void GameScene::Draw()
{
	player_->Draw(camera_, playerTex);
}


void GameScene::PostDraw()
{
	postProcess_->Draw();
}

void GameScene::Release() {
}
