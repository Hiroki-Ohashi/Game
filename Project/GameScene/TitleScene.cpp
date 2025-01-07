#include "TitleScene.h"

/// <summary>
/// TileScene.cpp
/// タイトルシーンのソースファイル
/// </summary>

TitleScene::~TitleScene()
{
}

void TitleScene::Initialize()
{
	camera_.Initialize();
	camera_.cameraTransform.translate = { 0.0f, 5.0f, -10.0f };
	camera_.cameraTransform.rotate.x = 0.4f;

	textureManager_->Initialize();

	postProcess_ = std::make_unique<PostProcess>();
	postProcess_->Initialize(NOISE);
	postProcess_->SetVignette(0.0f, 16.0f);

	// skybox
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();

	start = textureManager_->Load("resources/log.png");
	white = textureManager_->Load("resources/white.png");

	title = textureManager_->Load("resources/title.png");

	title_ = std::make_unique<Sprite>();
	title_->Initialize(Vector2{ 60.0f, 60.0f }, Vector2{ 90.0f, 80.0f }, title);

	startLog_ = std::make_unique<Sprite>();
	startLog_->Initialize(Vector2{ 0.0f, 0.0f }, Vector2{ 100.0f, 100.0f }, start);

	json_ = std::make_unique<Json>();
	levelData_ = json_->LoadJson("title");
	json_->Adoption(levelData_, true);

	blinking = true;
}


void TitleScene::Update()
{
	camera_.Update();
	json_->Update();
	
	postProcess_->NoiseUpdate(0.1f);

	if (input_->TriggerKey(DIK_A)) {
		isVignette_ = true;
	}

	XINPUT_STATE joyState;

	if (Input::GetInsTance()->GetJoystickState(joyState)) {

		if (Input::GetInsTance()->PressedButton(joyState, XINPUT_GAMEPAD_A)) {
			isVignette_ = true;
		}

	}

	if (isVignette_) {
		postProcess_->VignetteFadeIn(0.1f, 0.1f);
	}
	else {
		postProcess_->VignetteFadeOut(0.1f, 0.1f, 16.0f, 1.0f);
	}

	if (postProcess_->GetVignetteLight() <= 0.0f) {
		isVignette_ = false;
		sceneNo = STAGE;
	}

	if (camera_.cameraTransform.rotate.x < kCameraMax.x) {
		cameraSpeedX += cameraMoveSpeed;
	}
	else if (camera_.cameraTransform.rotate.x >= kCameraMax.x) {
		cameraSpeedX -= cameraMoveSpeed;
	}

	if (camera_.cameraTransform.rotate.y < kCameraMax.y) {
		cameraSpeedY += cameraMoveSpeed;
	}
	else if (camera_.cameraTransform.rotate.y >= kCameraMax.y) {
		cameraSpeedY -= cameraMoveSpeed;
	}

	camera_.cameraTransform.rotate.x += cameraSpeedX;
	camera_.cameraTransform.rotate.y += cameraSpeedY;

	timer += timerSpeed;

	if (timer == kMaxTimer) {
		timer = 0;
		if (blinking) {
			blinking = false;
		}
		else {
			blinking = true;
		}
	}

	if (ImGui::TreeNode("Sprite")) {
		ImGui::DragFloat2("Transform", &startLog_->GetPos().x, 0.1f, -1000.0f, 1000.0f);
		ImGui::DragFloat2("Scale", &transformSprite.scale.x, 0.1f, -1000.0f, 1000.0f);
		ImGui::TreePop();
	}
}

void TitleScene::Draw()
{
	skydome_->Draw(&camera_);

	title_->Draw();

	if (blinking) {
		startLog_->Draw();
	}

	json_->Draw(camera_, white);
}

void TitleScene::PostDraw()
{
	postProcess_->NoiseDraw();
}
