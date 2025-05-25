#include "ClearScene.h"

/// <summary>
/// ClearScene.cpp
/// クリアシーンのソースファイル
/// </summary>

ClearScene::~ClearScene()
{
}

void ClearScene::Initialize()
{
	railCamera_ = std::make_unique<RailCamera>();
	railCamera_->Initialize();

	// PostEffect
	postProcess_ = std::make_unique<PostProcess>();
	postProcess_->Initialize(NOISE);
	postProcess_->SetVignette(16.0f, 1.0f);
	postProcess_->SetNoise(0.2f, 100.0f);

	// skybox
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();

	// texture
	log = textureManager_->Load("resources/log.png");
	player = textureManager_->Load("resources/white.png");
	clear = textureManager_->Load("resources/clear.png");
	clearLog = textureManager_->Load("resources/clearLog.png");

	// UI(clear)
	clear_ = std::make_unique<Sprite>();
	clear_->Initialize(Vector2{ 170.0f, 530.0f }, Vector2{ 960.0f, 130.0f }, clear);

	// UI(log)
	log_ = std::make_unique<Sprite>();
	log_->Initialize(Vector2{ 1110.0f, 565.0f }, Vector2{ 15.0f, 70.0f }, log);
	log_->SetSize({ 15.0f, 50.0f });

	// UI(clearLog)
	clearLog_ = std::make_unique<Sprite>();
	clearLog_->Initialize(Vector2{ 310.0f, 50.0f }, Vector2{ 650.0f, 200.0f }, clearLog);

	// json
	json_ = std::make_unique<Json>();
	levelData_ = json_->LoadJson("clear");
	json_->Adoption(levelData_, true);
}

void ClearScene::Update()
{
	railCamera_->Update();

	// ポストエフェクト更新処理
	postProcess_->NoiseUpdate(0.1f);

	// json更新処理
	json_->Update();

	// ノイズを段々薄く
	if (postProcess_->GetNoiseStrength() > 1.0f) {
		noiseStrength += 1.0f;
		postProcess_->SetNoiseStrength(postProcess_->GetNoiseStrength() - noiseStrength);
	}

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

	if (isVignette_) {
		// フェードイン
		postProcess_->VignetteFadeIn(0.1f, 0.1f);
	}
	// タイトルシーンへ
	if (postProcess_->GetVignetteLight() <= 0.0f) {
		isVignette_ = false;
		sceneNo = TITLE;
	}

	// カメラ回転
	railCamera_->ClearCameraMove();

	// UI点滅
	Blinking();
}

void ClearScene::Draw()
{
	// 天球描画
	skydome_->Draw(railCamera_->GetCamera());

	// Json描画
	json_->Draw(railCamera_->GetCamera(), player);

	// UI描画
	clear_->Draw();
	clearLog_->Draw();

	if (blinking) {
		log_->Draw();
	}
}

void ClearScene::PostDraw()
{
	postProcess_->NoiseDraw();
}

void ClearScene::Blinking()
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