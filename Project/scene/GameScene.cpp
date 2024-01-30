#include "GameScene.h"

GameScene::~GameScene(){
	delete camera_;
	delete title_;
	delete kakusi_;
	delete ui_;
	delete skydome_;
}

void GameScene::Initialize(){
	textureManager_ = TextureManager::GetInstance();
	textureManager_->Initialize();

	input_ = Input::GetInsTance();

	camera_ = new Camera();
	camera_->Initialize();

	player_ = new Player();
	player_->Init();

	enemy_ = new Enemy();
	enemy_->Init();
	// 敵キャラに自キャラのアドレスを渡す
	enemy_->SetPlayer(player_);

	title_ = new Sprite();
	title_->Initialize({0,0}, {1280, 720}, 1.0f);

	kakusi_ = new Sprite();
	kakusi_->Initialize({ 0,0 }, { 1280, 720 }, 0.0f);

	ui_ = new Sprite();
	ui_->Initialize({ 460,360 }, { 360, 180 }, 0.01f);

	// 天球の生成
	skydome_ = new Skydome();
	// 天球の初期化
	skydome_->Initialize();

	uv = textureManager_->Load("Resources/start.png");
	kuro = textureManager_->Load("Resources/kuro.png");
	teki= textureManager_->Load("Resources/moon.png");
	ui = textureManager_->Load("Resources/ui.png");
	sky = textureManager_->Load("Resources/sky.png");

	a = 0.0f;
	isAAA = false;
	isTitle = true;

	scene = 0;
}

void GameScene::Update(){
	camera_->Update();


	if (scene == 0) {
		if (input_->TriggerKey(DIK_SPACE)) {
			isAAA = true;
		}

		if (isAAA) {
			a += speed;
		}

		if (a >= 1.2f) {
			speed *= -1;
			isTitle = false;
		}

		b += speed2;

		if (b >= 1.0f) {
			speed2 *= -1;
		}
		if (b <= 0.0f) {
			speed2 *= -1;
		}

		if (a <= -0.01f) {
			scene = 1;
		}
	}

	kakusi_->Update(a);
	ui_->Update(b);


	if (scene == 1) {
		player_->Update();

		// 敵キャラの更新
		if (enemy_ != nullptr) {
			enemy_->Update();
		}

		camera_->cameraTransform.translate.z -= 0.05f;
	}
}

void GameScene::Draw(){
	player_->Draw(kuro, camera_, kuro);
	enemy_->Draw(teki, camera_, kuro);
	skydome_->Draw(camera_, sky);

	if (isTitle) {
		title_->Draw(uv);
		ui_->Draw(ui);
	}
	
	kakusi_->Draw(kuro);
}

void GameScene::Release() {
}
