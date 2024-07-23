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

	postProcess_->Initialize(GRAY);

	transform = { { 1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,-1.0f,3.0f} };

	model_ = std::make_unique<AnimationModel>();
	model_->Initialize("walk.gltf", transform, camera_, 0);

	skyBox_ = std::make_unique<SkyBox>();
	skyBox_->Initialize();

	uv = textureManager_->Load("resources/uvChecker.png");
	skyTex = textureManager_->Load("resources/rostock_laage_airport_4k.dds");
}

void GameScene::Update(){

	camera_->Update();

	if (input_->TriggerKey(DIK_SPACE)) {
		sceneNo = TITLE;
	}
	
	if (input_->PushKey(DIK_D)) {
		camera_->cameraTransform.translate.x += 0.1f;
	}
	if (input_->PushKey(DIK_A)) {
		camera_->cameraTransform.translate.x -= 0.1f;
	}
	if (input_->PushKey(DIK_W)) {
		camera_->cameraTransform.translate.y += 0.1f;
	}
	if (input_->PushKey(DIK_S)) {
		camera_->cameraTransform.translate.y -= 0.1f;
	}

	if (input_->PushKey(DIK_1)) {
		postProcess_->Initialize(GRAY);
	}
	if (input_->PushKey(DIK_2)) {
		postProcess_->Initialize(VIGNETTE);
	}
	if (input_->PushKey(DIK_3)) {
		postProcess_->Initialize(BOX);
	}
	if (input_->PushKey(DIK_4)) {
		postProcess_->Initialize(GAUSSIAN);
	}

	model_->Update(1.0f);
}

void GameScene::Draw(){

	skyBox_->Draw(camera_, skyTex);

	model_->Draw(camera_, uv, skyTex);
}


void GameScene::PostDraw()
{
	postProcess_->Draw();
}

void GameScene::Release() {
}
