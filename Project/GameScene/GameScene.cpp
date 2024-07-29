#include "GameScene.h"

GameScene::~GameScene(){
	delete camera_;
}

void GameScene::Initialize(){
	textureManager_ = TextureManager::GetInstance();

	camera_ = new Camera();
	camera_->Initialize();


	postProcess_ = std::make_unique<PostProcess>();
	postProcess_->Initialize(VIGNETTE);

	transform = { { 1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,-1.0f,3.0f} };

	model2_ = std::make_unique<AnimationModel>();
	model2_->Initialize("walk.gltf", transform);

	json_ = std::make_unique<Json>();
	levelData_ = json_->LoadJson("level");
	json_->Adoption(levelData_);

	uv = textureManager_->Load("resources/uvChecker.png");
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


	//model2_->Update(1.0f);
}

void GameScene::Draw(){

	//model2_->Draw(camera_, uv);
	json_->Draw(*camera_, uv);
}


void GameScene::PostDraw()
{
	postProcess_->Draw();
}

void GameScene::Release() {
}
