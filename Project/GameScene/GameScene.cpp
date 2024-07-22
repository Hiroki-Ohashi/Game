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

}

void GameScene::Update(){

	camera_->Update();

	if (input_->TriggerKey(DIK_SPACE)) {
		sceneNo = TITLE;
	}
}

void GameScene::Draw(){
}


void GameScene::PostDraw()
{
	postProcess_->Draw();
}

void GameScene::Release() {
}
