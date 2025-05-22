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
	railCamera_ = std::make_unique<RailCamera>();
	railCamera_->Initialize();
	railCamera_->SetPos({ 0.0f, 5.0f, -10.0f });
	railCamera_->SetRot({ 0.4f, 0.0f, 0.0f });
	railCamera_->SetkCameraMax({ 0.40f , 0.0f });

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

	l = textureManager_->Load("resources/l.png");
	lo = textureManager_->Load("resources/lo.png");
	loa = textureManager_->Load("resources/loa.png");
	load = textureManager_->Load("resources/load.png");
	loadi = textureManager_->Load("resources/loadi.png");
	loadin = textureManager_->Load("resources/loadin.png");
	loading = textureManager_->Load("resources/loading.png");
	loading1 = textureManager_->Load("resources/loading..png");
	loading2 = textureManager_->Load("resources/loading...png");
	loading3 = textureManager_->Load("resources/loading....png");

	// UI(title)
	title_ = std::make_unique<Sprite>();
	title_->Initialize(Vector2{ 60.0f, 80.0f }, Vector2{ 500.0f, 370.0f }, title);

	// UI(startLog)
	startLog_ = std::make_unique<Sprite>();
	startLog_->Initialize(Vector2{ 490.0f, 290.0f }, Vector2{ 14.0f, 42.0f }, start);
	startLog_->SetSize({ 14.0f, 42.0f });

	// Load
	loadSprite_ = std::make_unique<Sprite>();
	loadSprite_->Initialize(Vector2{ 0.0f, 0.0f }, Vector2{ 1280.0f, 720.0f }, l);
	Loadtimer = 0;

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
	railCamera_->Update();
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

		Loadtimer += 1;

		if (Loadtimer == 10) {
			loadSprite_->SetTexture(lo);
		}
		else if (Loadtimer == 15) {
			loadSprite_->SetTexture(loa);
		}
		else if (Loadtimer == 20) {
			loadSprite_->SetTexture(load);
		}
		else if (Loadtimer == 25) {
			loadSprite_->SetTexture(loadi);
		}
		else if (Loadtimer == 30) {
			loadSprite_->SetTexture(loadin);
		}
		else if (Loadtimer == 35) {
			loadSprite_->SetTexture(loading);
		}
		else if (Loadtimer == 40) {
			loadSprite_->SetTexture(loading1);
		}
		else if (Loadtimer == 45) {
			loadSprite_->SetTexture(loading2);
		}
		else if (Loadtimer >= 50) {
			loadSprite_->SetTexture(loading3);
			sceneNo = STAGE;
		}
	}
	
	// シーン遷移
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

	// カメラ揺らす
	railCamera_->CameraSwing();

	// UI点滅
	Blinking();
}

void TitleScene::Draw()
{
	// 天球描画
	skydome_->Draw(railCamera_->GetCamera());
	// Json描画
	json_->Draw(railCamera_->GetCamera(), white);

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
