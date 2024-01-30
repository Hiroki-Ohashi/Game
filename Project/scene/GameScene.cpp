#include "GameScene.h"

GameScene::~GameScene(){
	delete camera_;
}

void GameScene::Initialize(){
	textureManager_ = TextureManager::GetInstance();

	camera_ = new Camera();
	camera_->Initialize();

	player_ = std::make_unique<Player>();
	player_->Init("cube.obj");

	uv = textureManager_->Load("Resources/uvChecker.png");
}

void GameScene::Update(){
	camera_->Update();

	player_->Update();
}

void GameScene::Draw(){
	player_->Draw(uv, camera_);
}

void GameScene::Release() {
}
