#include "GameScene.h"

GameScene::~GameScene(){
	/*for (int i = 0; i < Max; i++) {
		delete triangle_[i];
	}*/
	delete camera_;
}

void GameScene::Initialize(){
	textureManager_ = TextureManager::GetInstance();
	textureManager_->Initialize();

	camera_ = new Camera();
	camera_->Initialize();

	sphere_ = std::make_unique<Sphere>();
	sphere_->Initialize();

	transform = { { 0.5f,0.5f,0.5f},{0.0f,0.0f,0.0f},{0.0f,0.0f,3.0f} };
	transform2 = { { 0.5f,0.5f,0.5f},{0.0f,0.0f,0.0f},{1.5f,-0.5f,3.0f} };

	model_ = std::make_unique<Model>();
	model_->Initialize("AnimatedCube.gltf", transform);

	//model2_ = std::make_unique<Model>();
	//model2_->Initialize("plane.obj", transform2);

	//particle_ = std::make_unique<Particles>();
	//particle_->Initialize("plane.obj", pos, 7);

	//particle2_ = std::make_unique<Particles>();
	//particle2_->Initialize("plane.obj", pos2, 8);

	//Vector4 pos[Max][3];

	//// 左下
	//pos[0][0] = { -0.5f, -0.25f, 0.0f, 1.0f };
	//// 上
	//pos[0][1] = { 0.0f, 0.5f, 0.0f, 1.0f };
	//// 右下
	//pos[0][2] = { 0.5f, -0.25f, 0.0f, 1.0f };

	//// 左下2
	//pos[1][0] = { -0.5f, -0.25f, 0.5f, 1.0f };
	//// 上2
	//pos[1][1] = { 0.0f, 0.0f, 0.0f, 1.0f };
	//// 右下2
	//pos[1][2] = { 0.5f, -0.25f, -0.5f, 1.0f };

	/*for (int i = 0; i < Max; i++) {
		triangle_[i] = new Triangle();
		triangle_[i]->Initialize(pos[i]);
	}*/

	uv = textureManager_->Load("resources/uvChecker.png");
	moon = textureManager_->Load("resources/moon.png");
	monsterBall = textureManager_->Load("resources/monsterball.png");
	kusa = textureManager_->Load("resources/kusa.png");
	circle = textureManager_->Load("resources/circle.png");
	utillity = textureManager_->Load("resources/AnimatedCube_BaseColor.png");
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
}

void GameScene::Draw(){

	//sphere_->Draw(camera_, moon);

	model_->Draw(camera_, utillity);
	//model2_->Draw(camera_, uv);

	//particle_->Draw(camera_, circle);
	//particle2_->Draw(camera_, uv);
}

void GameScene::Release() {
}
