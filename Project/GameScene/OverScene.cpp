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
	railCamera_ = std::make_unique<RailCamera>();
	railCamera_->Initialize();
	railCamera_->SetPos({ -0.4f, 7.0f, -19.0f });
	railCamera_->SetRot({ 0.25f, 0.0f, 0.0f });
	railCamera_->SetkCameraMax({ 0.25f , 0.0f });

	
	// PostEffect
	postProcess_ = std::make_unique<PostProcess>();
	postProcess_->Initialize(NOISE);
	postProcess_->SetVignette(16.0f, 1.0f);
	postProcess_->SetNoise(0.2f, noiseStrength);

	// json
	json_ = std::make_unique<Json>();
	levelData_ = json_->LoadJson("over");
	json_->Adoption(levelData_, true);

	// Objectjson
	jsonObject_ = std::make_unique<Json>();
	levelDataObject_ = jsonObject_->LoadJson("overObject");
	jsonObject_->Adoption(levelDataObject_, true);

	// stage
	transform_ = { {1000.0f,0.1f,1000.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	yuka_ = std::make_unique<Model>();
	yuka_->Initialize("cube.obj", transform_);
	yuka_->SetLight(false);

	// texture
	player = textureManager_->Load("resources/white.png");
	yuka = textureManager_->Load("resources/map.png");
	gekitui = textureManager_->Load("resources/gekitui.png");
	sareta = textureManager_->Load("resources/sareta.png");

	// UI(gekitui)
	gekitui_ = std::make_unique<Sprite>();
	gekitui_->Initialize(Vector2{ 330.0f, 30.0f }, Vector2{ 331.0f, 190.0f }, gekitui);
	gekitui_->SetSize({ 331.0f, 190.0f });

	// UI(sareta)
	sareta_ = std::make_unique<Sprite>();
	sareta_->Initialize(Vector2{ 685.0f, 121.0f }, Vector2{ 1.0f, 1.0f }, sareta);
	sareta_->SetSize({ 320.0f, 100.0f });

	// texture
	sentaku = textureManager_->Load("resources/sentaku.png");
	retry = textureManager_->Load("resources/retry.png");
	title = textureManager_->Load("resources/backTitle.png");

	// UI(sentaku)
	sentaku_ = std::make_unique<Sprite>();
	sentaku_->Initialize(Vector2{ 1050.0f, 560.0f }, Vector2{ 127.0f, 107.0f }, sentaku);
	sentaku_->SetSize({ 127.0f, 107.0f });

	// UI(retry)
	retry_ = std::make_unique<Sprite>();
	retry_->Initialize(Vector2{ 1050.0f, 560.0f }, Vector2{ 127.0f, 107.0f }, retry);
	retry_->SetSize({ 127.0f, 107.0f });

	// UI(title)
	title_ = std::make_unique<Sprite>();
	title_->Initialize(Vector2{ 1050.0f, 560.0f }, Vector2{ 127.0f, 107.0f }, title);
	title_->SetSize({ 127.0f, 107.0f });

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

	// Load
	loadSprite_ = std::make_unique<Sprite>();
	loadSprite_->Initialize(Vector2{ 0.0f, 0.0f }, Vector2{ 1280.0f, 720.0f }, l);
	Loadtimer = 0;
	isLoad_ = false;

	scenePrev = 0;
	noiseStrength = 100.0f;
}

void OverScene::Update()
{
	railCamera_->Update();

	// ノイズを段々薄く
	if (noiseStrength >= 5.0f) {
		noiseStrength -= 1.0f;
		postProcess_->SetNoiseStrength(noiseStrength);
	}

	// ポストエフェクト更新処理
	postProcess_->NoiseUpdate(0.1f);

	// json更新処理
	json_->Update();
	jsonObject_->Update();

	XINPUT_STATE joyState;

	// 十字キーでシーン選択
	if (Input::GetInstance()->GetJoystickState(joyState)) {
		// 長押し防止
		if (scenePrev == 0) {
			if (Input::GetInstance()->PressedButton(joyState, XINPUT_GAMEPAD_DPAD_DOWN)) {
				scenePrev = 1;
			}
		}
		else if (scenePrev == 1) {
			if (Input::GetInstance()->PressedButton(joyState, XINPUT_GAMEPAD_DPAD_UP)) {
				scenePrev = 0;
			}
		}

		// 選んだシーンをAボタンで遷移開始
		if (scenePrev == 0) {
			if (Input::GetInstance()->PressedButton(joyState, XINPUT_GAMEPAD_A)) {
				isVignette_ = true;
			}
		}else if (scenePrev == 1) {
			if (Input::GetInstance()->PressedButton(joyState, XINPUT_GAMEPAD_A)) {
				isVignette_ = true;
			}
		}

	}

	// 選んだシーンでフェードイン
	if (scenePrev == 0) {
		if (isVignette_) {
			postProcess_->VignetteFadeIn(0.1f, 0.1f);
		}

		if (postProcess_->GetVignetteLight() <= 0.0f) {
			isVignette_ = false;
			isLoad_ = true;
		}
	}
	else {
		if (isVignette_) {
			postProcess_->VignetteFadeIn(0.1f, 0.1f);
		}

		if (postProcess_->GetVignetteLight() <= 0.0f) {
			isVignette_ = false;
			// タイトルシーンへ
			sceneNo = TITLE;
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
			// ゲームシーンへ
			sceneNo = STAGE;
		}
	}

	// カメラ揺らす
	railCamera_->CameraSwing();

	// UI点滅
	Blinking();

	if (ImGui::TreeNode("Scene")) {
		ImGui::Text("Scene num = %d", scenePrev);
		ImGui::TreePop();
	}
}

void OverScene::Draw()
{
	// Json描画
	json_->Draw(railCamera_->GetCamera(), player);
	jsonObject_->Draw(railCamera_->GetCamera(), player);

	// 床描画
	yuka_->Draw(railCamera_->GetCamera(), yuka);

	if (isLoad_) {
		loadSprite_->Draw();
	}
	else {
		// UI描画
		gekitui_->Draw();
		sareta_->Draw();
		sentaku_->Draw();
		if (blinking) {
			if (scenePrev == 0) {
				retry_->Draw();
			}
			else {
				title_->Draw();
			}
		}
	}
}

void OverScene::PostDraw()
{
	postProcess_->NoiseDraw();
}

void OverScene::Blinking()
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
