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

	postProcess_ = std::make_unique<PostProcess>();
	postProcess_->Initialize();
  
	/*sphere_ = std::make_unique<Sphere>();
	sphere_->Initialize();*/

	transform = { { 1.0f,1.0f,1.0f},{0.0f,3.0f,0.0f},{1.0f,0.0f,0.0f} };
	transform2 = { { 1.0f,1.0f,1.0f},{0.0f,3.0f,0.0f},{0.0f,0.0f,0.0f} };
	transform3 = { { 1.0f,1.0f,1.0f},{0.0f,3.0f,0.0f},{-1.0f,0.0f,0.0f} };
	transform4 = { { 0.5f,0.5f,0.5f},{0.0f,0.0f,0.0f},{0.0f,-1.0f,0.0f} };

	model_ = std::make_unique<AnimationModel>();
	model_->Initialize("simpleSkin.gltf", transform, camera_, 0);

	model2_ = std::make_unique<AnimationModel>();
	model2_->Initialize("walk.gltf", transform2, camera_, 1);

	model3_ = std::make_unique<AnimationModel>();
	model3_->Initialize("sneakWalk.gltf", transform3, camera_, 2);

	//particle_ = std::make_unique<Particles>();
	//particle_->Initialize("plane.obj", pos, 7);

	//particle2_ = std::make_unique<Particles>();
	//particle2_->Initialize("plane.obj", pos2, 8);

	skyBox_ = std::make_unique<SkyBox>();
	skyBox_->Initialize();


	model4_ = std::make_unique<Model>();
	model4_->Initialize("cube.obj", transform4);

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
	white = textureManager_->Load("resources/white.png");

	skybox = textureManager_->Load("resources/rostock_laage_airport_4k.dds");
}

void GameScene::Update(){

	camera_->Update();

	if (input_->TriggerKey(DIK_SPACE)) {
		sceneNo = TITLE;
	}
	
	if (input_->PushKey(DIK_D)) {
		transform2.translate.x += speed;
	}
	if (input_->PushKey(DIK_A)) {
		transform2.translate.x -= speed;
	}
	if (input_->PushKey(DIK_W)) {
		transform2.translate.z += speed;
	}
	if (input_->PushKey(DIK_S)) {
		transform2.translate.z -= speed;
	}

	angle = std::atan2(transform2.translate.x, transform2.translate.z);
	transform2.rotate.y = LerpShortAngle(transform2.rotate.y, angle, 1.0f);

	// ゲームパッドの状態を得る変数(XINPUT)
	XINPUT_STATE joyState;

	if (Input::GetInsTance()->GetJoystickState(joyState)) {
		const float sikii = 5.0f;
		bool isMoving = false;
		//
		Vector3 move = { (float)joyState.Gamepad.sThumbLX, 0.0f, (float)joyState.Gamepad.sThumbLY };
		if (Length(move) > sikii) {
			isMoving = true;
		}
		if (isMoving) {
			transform2.translate.x += (float)joyState.Gamepad.sThumbLX / SHRT_MAX * speed;
			transform2.translate.z += (float)joyState.Gamepad.sThumbLY / SHRT_MAX * speed;
			angle = std::atan2(move.x, move.z);
		}
	}
	transform2.rotate.y = LerpShortAngle(transform2.rotate.y, angle, 1.0f);
	model2_->SetTranslate(transform2.translate);
	model2_->SetRotate(transform2.rotate);


	model_->Update(6.0f);
	model2_->Update(1.0f);
	model3_->Update(0.9f);

}

void GameScene::Draw(){

	/*sphere_->Draw(camera_, moon);*/

	model4_->Draw(camera_, uv);

	skyBox_->Draw(camera_, skybox);

	model_->Draw(camera_, uv, skybox);
	model2_->Draw(camera_, white, skybox);
	model3_->Draw(camera_, uv, skybox);

	//particle_->Draw(camera_, circle);
	//particle2_->Draw(camera_, uv);
}


void GameScene::PostDraw()
{
	postProcess_->Draw();
}

void GameScene::Release() {
}
