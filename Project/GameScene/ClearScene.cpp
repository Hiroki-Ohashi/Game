#include "ClearScene.h"

ClearScene::~ClearScene()
{
}

void ClearScene::Initialize()
{
	camera_.Initialize();
	textureManager_->Initialize();

	postProcess_ = std::make_unique<PostProcess>();
	postProcess_->Initialize(NOISE);
	postProcess_->SetVignette(16.0f, 1.0f);
	postProcess_->SetNoise(0.2f, 100.0f);

	// skybox
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();

	clear_ = std::make_unique<Sprite>();
	clear_->Initialize(Vector2{ 150.0f, 300.0f }, Vector2{ 700.0, 70.0f }, 1.0f);

	log_ = std::make_unique<Sprite>();
	log_->Initialize(Vector2{ 492.0f, 308.0f }, Vector2{ 15.0f, 35.0f }, 1.0f);

	clearLog_ = std::make_unique<Sprite>();
	clearLog_->Initialize(Vector2{ 200.0f, 25.0f }, Vector2{ 450.0, 150.0f }, 1.0f);

	clear = textureManager_->Load("resources/clear.png");
	clearLog = textureManager_->Load("resources/clearLog.png");
	log = textureManager_->Load("resources/log.png");
	player = textureManager_->Load("resources/white.png");

	json_ = std::make_unique<Json>();
	levelData_ = json_->LoadJson("clear");
	json_->Adoption(levelData_, true);
}

void ClearScene::Update()
{
	camera_.Update();

	postProcess_->NoiseUpdate(0.1f);

	json_->Update();

	if (postProcess_->GetNoiseStrength() > 1.0f) {
		noiseStrength += 1.0f;
		postProcess_->SetNoiseStrength(postProcess_->GetNoiseStrength() - noiseStrength);
	}

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

	if (postProcess_->GetVignetteLight() <= 0.0f) {
		isVignette_ = false;
		sceneNo = TITLE;
	}

	camera_.cameraTransform.rotate.y += 0.01f;

	EulerTransform origin = { {0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	// 追従対象からカメラまでのオフセット
	Vector3 offset = { 0.0f, 13.0f, -25.0f };
	// カメラの角度から回転行列を計算する
	Matrix4x4 worldTransform = MakeRotateYMatrix(camera_.cameraTransform.rotate.y);
	// オフセットをカメラの回転に合わせて回転させる
	offset = TransformNormal(offset, worldTransform);
	// 座標をコピーしてオフセット分ずらす
	camera_.cameraTransform.translate.x = origin.translate.x + offset.x;
	camera_.cameraTransform.translate.y = origin.translate.y + offset.y;
	camera_.cameraTransform.translate.z = origin.translate.z + offset.z;

	camera_.cameraTransform.rotate.x = 0.4f;

	timer += 1;

	if (timer == 30) {
		timer = 0;
		if (blinking) {
			blinking = false;
		}
		else {
			blinking = true;
		}
	}
}

void ClearScene::Draw()
{
	clear_->Draw(clear);
	clearLog_->Draw(clearLog);

	if (blinking) {
		log_->Draw(log);
	}

	skydome_->Draw(&camera_);

	json_->Draw(camera_, player);
}

void ClearScene::PostDraw()
{
	postProcess_->NoiseDraw();
}
