#include "GameScene.h"

GameScene::~GameScene(){
	delete camera_;
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

}

void GameScene::Update(){

	camera_->Update();
	player_->Update();
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
	stage_->Draw(camera_);
}


void GameScene::PostDraw()
{
	postProcess_->Draw();
}

void GameScene::Release() {
}
