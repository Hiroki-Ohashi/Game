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

	postProcess_ = std::make_unique<PostProcess>();
	postProcess_->Initialize(NOISE);
	postProcess_->SetVignette(0.0f, 16.0f);

	// skybox
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();

	// texture
	start = textureManager_->Load("resources/log.png");
	white = textureManager_->Load("resources/white.png");
	title = textureManager_->Load("resources/title.png");
	load = textureManager_->Load("resources/loading.png");

	// UI(title)
	title_ = std::make_unique<Sprite>();
	title_->Initialize(Vector2{ 60.0f, 80.0f }, Vector2{ 500.0f, 370.0f }, title);

	// UI(startLog)
	startLog_ = std::make_unique<Sprite>();
	startLog_->Initialize(Vector2{ 490.0f, 290.0f }, Vector2{ 14.0f, 42.0f }, start);
	startLog_->SetSize({ 14.0f, 42.0f });

	// Load
	loadSprite_ = std::make_unique<Sprite>();
	loadSprite_->Initialize(Vector2{ 0.0f, 0.0f }, Vector2{ 1280.0f, 720.0f }, load);

	// Json
	json_ = std::make_unique<Json>();
	levelData_ = json_->LoadJson("title");
	json_->Adoption(levelData_, true);

	// boolInit
	blinking = true;
	isLoad_ = false;
}


void TitleScene::Update()
{
	camera_.Update();
	json_->Update();
	
	postProcess_->NoiseUpdate(0.1f);

	// キーボード
	if (input_->TriggerKey(DIK_A)) {
		isVignette_ = true;
	}

	XINPUT_STATE joyState;

	// Aボタンで遷移
	if (Input::GetInstance()->GetJoystickState(joyState)) {

		if (Input::GetInstance()->PressedButton(joyState, XINPUT_GAMEPAD_A)) {
			isVignette_ = true;
		}

	}

	if (isLoad_) {
		postProcess_->SetVignette(32.0f, 1.0f);
		postProcess_->SetNoise(0.0f, 0.0f);
		sceneNo = STAGE;
	}
	else {
		if (isVignette_) {
			// フェードイン
			postProcess_->VignetteFadeIn(0.1f, 0.1f);
		}
		else {
			//フェードアウト
			postProcess_->VignetteFadeOut(0.1f, 0.1f, 16.0f, 1.0f);
		}

		// ゲームシーンへ
		if (postProcess_->GetVignetteLight() <= 0.0f) {
			isLoad_ = true;
			isVignette_ = false;
		}
	}

	// カメラ揺らす
	CameraShake();

	// UI点滅
	Blinking();
}

void TitleScene::Draw()
{
	// 天球描画
	skydome_->Draw(&camera_);
	// Json描画
	json_->Draw(camera_, white);

	if (isLoad_) {
		loadSprite_->Draw();
	}
	else {
		// UI描画
		title_->Draw();

		if (blinking) {
			startLog_->Draw();
		}
	}
}

void TitleScene::PostDraw()
{
	postProcess_->NoiseDraw();
}

void TitleScene::CameraShake()
{
	// カメラ角度が範囲を超えたら反転
	// X軸
	if (camera_.cameraTransform.rotate.x < kCameraMax.x) {
		cameraSpeedX += cameraMoveSpeed;
	}
	else if (camera_.cameraTransform.rotate.x >= kCameraMax.x) {
		cameraSpeedX -= cameraMoveSpeed;
	}
	// Y軸
	if (camera_.cameraTransform.rotate.y < kCameraMax.y) {
		cameraSpeedY += cameraMoveSpeed;
	}
	else if (camera_.cameraTransform.rotate.y >= kCameraMax.y) {
		cameraSpeedY -= cameraMoveSpeed;
	}
	// カメラスピードを足す
	camera_.cameraTransform.rotate.x += cameraSpeedX;
	camera_.cameraTransform.rotate.y += cameraSpeedY;
}

void TitleScene::Blinking()
{
	timer += timerSpeed;

	// タイマーで点滅速度を調整
	if (timer == kMaxTimer) {
		timer = 0;
		if (blinking) {
			blinking = false;
		}
		else {
			blinking = true;
		}
	}
}
