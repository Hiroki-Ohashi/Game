#include "GameScene.h"

GameScene::~GameScene(){
}

void GameScene::Initialize(){
	textureManager_ = TextureManager::GetInstance();
	textureManager_->Initialize();

	camera_ = std::make_unique<Camera>();
	camera_->Initialize();

	
}

void GameScene::Update(){

	camera_->Update();

}

void GameScene::Draw(){
}


void GameScene::PostDraw()
{

}

void GameScene::Release() {
}
