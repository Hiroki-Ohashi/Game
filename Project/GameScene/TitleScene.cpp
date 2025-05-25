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
	// カメラ初期化と設定
	railCamera_ = std::make_unique<RailCamera>();
	railCamera_->Initialize();
	railCamera_->SetPos({ 0.0f, 5.0f, -10.0f });
	railCamera_->SetRot({ 0.4f, 0.0f, 0.0f });
	railCamera_->SetkCameraMax({ 0.40f , 0.0f });

	// ポストプロセス初期化
	postProcess_ = std::make_unique<PostProcess>();
	postProcess_->Initialize(NOISE);
	postProcess_->SetVignette(0.0f, 16.0f);

	// skybox
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();

	// テクスチャ読み込み
	start = textureManager_->Load("resources/log.png");
	white = textureManager_->Load("resources/white.png");
	title = textureManager_->Load("resources/title.png");

	// Load初期化
	loadingManager_ = std::make_unique<LoadingManager>();
	loadingManager_->Initialize(textureManager_);

	// UI(title)
	title_ = std::make_unique<Sprite>();
	title_->Initialize(Vector2{ 60.0f, 80.0f }, Vector2{ 500.0f, 370.0f }, title);

	// UI(startLog)
	startLog_ = std::make_unique<Sprite>();
	startLog_->Initialize(Vector2{ 490.0f, 290.0f }, Vector2{ 14.0f, 42.0f }, start);
	startLog_->SetSize({ 14.0f, 42.0f });

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

	// キーボードまたはゲームパッドの入力でビネット開始
	if (input_->TriggerKey(DIK_A)) {
		isVignette_ = true;
	}

	XINPUT_STATE joyState;
	if (Input::GetInstance()->GetJoystickState(joyState)) {
		if (Input::GetInstance()->PressedButton(joyState, XINPUT_GAMEPAD_A)) {
			isVignette_ = true;
		}
	}

	// ポストエフェクトのフェード制御
	if (isVignette_) {
		postProcess_->VignetteFadeIn(0.1f, 0.1f);
	}
	else {
		postProcess_->VignetteFadeOut(0.1f, 0.1f, 16.0f, 1.0f);
	}

	// ビネットが真っ黒になったらロード開始
	if (postProcess_->GetVignetteLight() <= 0.0f && !isLoad_) {
		isLoad_ = true;
		isVignette_ = false;
		loadingManager_->Start();
	}

	// ローディング中は演出に切り替え
	if (isLoad_) {
		postProcess_->SetVignette(32.0f, 1.0f);  // 画面暗く
		postProcess_->SetNoise(0.0f, 0.0f);      // ノイズ解除
		loadingManager_->Update();
	}

	// ロード完了したらステージシーンへ
	if (loadingManager_->IsComplete()) {
		sceneNo = STAGE;
	}

	// カメラ揺らし
	railCamera_->CameraSwing();

	// ロゴ点滅
	Blinking();
}

void TitleScene::Draw()
{
	// 天球描画
	skydome_->Draw(railCamera_->GetCamera());
	// Json描画
	json_->Draw(railCamera_->GetCamera(), white);

	if (isLoad_) {
		loadingManager_->Draw();
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