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
	camera_.Initialize();
	textureManager_->Initialize();

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
	clear_->Initialize(Vector2{ 315.0f, 566.0f }, Vector2{ 1.0f, 1.0f }, clear);

	// UI(log)
	log_ = std::make_unique<Sprite>();
	log_->Initialize(Vector2{ 987.0f, 582.0f }, Vector2{ 14.0f, 42.0f }, log);
	log_->SetSize({ 14.0f, 32.0f });

	// UI(clearLog)
	clearLog_ = std::make_unique<Sprite>();
	clearLog_->Initialize(Vector2{ 415.0f, 70.0f }, Vector2{ 1.0f, 1.0f }, clearLog);

	// json
	json_ = std::make_unique<Json>();
	levelData_ = json_->LoadJson("clear");
	json_->Adoption(levelData_, true);
}

void ClearScene::Update()
{
	camera_.Update();

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
	if (Input::GetInsTance()->GetJoystickState(joyState)) {

		if (Input::GetInsTance()->PressedButton(joyState, XINPUT_GAMEPAD_A)) {
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
	CameraMove();

	// UI点滅
	Blinking();
}

void ClearScene::Draw()
{
	// 天球描画
	skydome_->Draw(&camera_);

	// Json描画
	json_->Draw(camera_, player);

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

void ClearScene::CameraMove()
{
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