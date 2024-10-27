#include "OverScene.h"

/// <summary>
/// OverScene.cpp
/// ゲームオーバーシーンのソースファイル
/// </summary>

OverScene::~OverScene()
{
}

void OverScene::Initialize()
{
	camera_.Initialize();
	camera_.cameraTransform.translate = { -0.4f, 7.0f, -19.0f };
	camera_.cameraTransform.rotate.x = 0.25f;

	textureManager_->Initialize();

	postProcess_ = std::make_unique<PostProcess>();
	postProcess_->Initialize(NOISE);
	postProcess_->SetVignette(16.0f, 1.0f);
	postProcess_->SetNoise(0.2f, noiseStrength);


	gekitui_ = std::make_unique<Sprite>();
	gekitui_->Initialize(Vector2{ 180.0f, 30.0f }, Vector2{ 257.0f, 137.0f }, 1.0f);

	sareta_ = std::make_unique<Sprite>();
	sareta_->Initialize(Vector2{ 320.0f, 80.0f }, Vector2{ 221.0f, 37.0f }, 1.0f);

	sentaku_ = std::make_unique<Sprite>();
	sentaku_->Initialize(Vector2{ 500.0f, 100.0f }, Vector2{ 127.0f, 107.0f }, 1.0f);

	json_ = std::make_unique<Json>();
	levelData_ = json_->LoadJson("over");
	json_->Adoption(levelData_, true);

	jsonObject_ = std::make_unique<Json>();
	levelDataObject_ = jsonObject_->LoadJson("overObject");
	jsonObject_->Adoption(levelDataObject_, true);


	transform_ = { {1000.0f,0.1f,1000.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	yuka_ = std::make_unique<Model>();
	yuka_->Initialize("cube.obj", transform_);
	yuka_->SetLight(false);

	gekitui = textureManager_->Load("resources/gekitui.png");
	sareta = textureManager_->Load("resources/sareta.png");

	sentaku = textureManager_->Load("resources/sentaku.png");
	retry = textureManager_->Load("resources/retry.png");
	title = textureManager_->Load("resources/backTitle.png");

	player = textureManager_->Load("resources/white.png");
	yuka = textureManager_->Load("resources/map.png");

	scenePrev = 0;
	noiseStrength = 100.0f;
}

void OverScene::Update()
{
	camera_.Update();

	if (noiseStrength >= 5.0f) {
		noiseStrength -= 1.0f;
		postProcess_->SetNoiseStrength(noiseStrength);
	}

	if (camera_.cameraTransform.rotate.x < 0.25f) {
		cameraSpeedX += 0.000005f;
	}
	else if (camera_.cameraTransform.rotate.x >= 0.25f) {
		cameraSpeedX -= 0.000005f;
	}

	if (camera_.cameraTransform.rotate.y < 0.0f) {
		cameraSpeedY += 0.000005f;
	}
	else if (camera_.cameraTransform.rotate.y >= 0.0f) {
		cameraSpeedY -= 0.000005f;
	}
	camera_.cameraTransform.rotate.x += cameraSpeedX;
	camera_.cameraTransform.rotate.y += cameraSpeedY;

	postProcess_->NoiseUpdate(0.1f);

	json_->Update();
	jsonObject_->Update();

	XINPUT_STATE joyState;

	if (Input::GetInsTance()->GetJoystickState(joyState)) {

		if (scenePrev == 0) {
			if (Input::GetInsTance()->PressedButton(joyState, XINPUT_GAMEPAD_DPAD_DOWN)) {
				scenePrev = 1;
			}
		}
		else if (scenePrev == 1) {
			if (Input::GetInsTance()->PressedButton(joyState, XINPUT_GAMEPAD_DPAD_UP)) {
				scenePrev = 0;
			}
		}


		if (scenePrev == 0) {
			if (Input::GetInsTance()->PressedButton(joyState, XINPUT_GAMEPAD_A)) {
				isVignette_ = true;
			}
		}else if (scenePrev == 1) {
			if (Input::GetInsTance()->PressedButton(joyState, XINPUT_GAMEPAD_A)) {
				isVignette_ = true;
			}
		}

	}

	if (scenePrev == 0) {
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
	}
	else {
		if (isVignette_) {
			postProcess_->VignetteFadeIn(0.1f, 0.1f);
		}
		else {
			postProcess_->VignetteFadeOut(0.1f, 0.1f, 16.0f, 1.0f);
		}

		if (postProcess_->GetVignetteLight() <= 0.0f) {
			isVignette_ = false;
			sceneNo = TITLE;
		}
	}


	timer += 1;

	if (timer == 20) {
		timer = 0;
		if (blinking) {
			blinking = false;
		}
		else {
			blinking = true;
		}
	}

	if (ImGui::TreeNode("Scene")) {
		ImGui::Text("Scene num = %d", scenePrev);
		ImGui::TreePop();
	}
}

void OverScene::Draw()
{
	gekitui_->Draw(gekitui);
	sareta_->Draw(sareta);

	sentaku_->Draw(sentaku);

	if (blinking) {
		if (scenePrev == 0) {
			sentaku_->Draw(retry);
		}
		else {
			sentaku_->Draw(title);
		}
	}

	json_->Draw(camera_, player);
	jsonObject_->Draw(camera_, player);

	yuka_->Draw(&camera_, yuka);
}

void OverScene::PostDraw()
{
	postProcess_->NoiseDraw();
}
