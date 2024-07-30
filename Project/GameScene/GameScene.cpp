#include "GameScene.h"

GameScene::~GameScene(){
}

void GameScene::Initialize(){
	textureManager_ = TextureManager::GetInstance();

	camera_.Initialize();


	postProcess_ = std::make_unique<PostProcess>();
	postProcess_->Initialize(VIGNETTE);

	json_ = std::make_unique<Json>();
	levelData_ = json_->LoadJson("level");
	json_->Adoption(levelData_);

	uv = textureManager_->Load("resources/uvChecker.png");
}

void GameScene::Update(){
	camera_.Update();
	json_->Update();
	camera_.cameraTransform = json_->GetCamera().cameraTransform;
}

void GameScene::Draw(){
	json_->Draw(camera_, uv);
}


void GameScene::PostDraw()
{
	postProcess_->Draw();
}

void GameScene::Release() {
}
