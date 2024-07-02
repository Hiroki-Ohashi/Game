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
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();

	// texture
	playerTex = textureManager_->Load("resources/white.png");
}

void GameScene::Update(){

	camera_->Update();
	player_->Update();

	camera_->cameraTransform.translate.z = player_->GetPos().z - 50.0f;

	if (input_->TriggerKey(DIK_RETURN)) {
		sceneNo = TITLE;
	}
}

void GameScene::Draw()
{
	skydome_->Draw(camera_);
	player_->Draw(camera_, playerTex);
}


void GameScene::PostDraw()
{
	postProcess_->Draw();
}

void GameScene::Release() {
}
