#include "TitleScene.h"

TitleScene::~TitleScene()
{
	delete camera_;
}

void TitleScene::Initialize()
{
	textureManager_->Initialize();

	camera_ = new Camera();
	camera_->Initialize();

	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize(Vector2{ 0.0f, 0.0f }, Vector2{ 300.0f, 300.0f });

	sprite2_ = std::make_unique<Sprite>();
	sprite2_->Initialize(Vector2{ 300.0f, 300.0f }, Vector2{ 300.0f, 300.0f });

	sphere_ = std::make_unique<Sphere>();
	sphere_->Initialize();

	uv = textureManager_->Load("resources/uvChecker.png");
	moon = textureManager_->Load("resources/moon.png");
	monsterBall = textureManager_->Load("resources/monsterball.png");
	kusa = textureManager_->Load("resources/kusa.png");
	circle = textureManager_->Load("resources/circle.png");
}


void TitleScene::Update()
{
	camera_->Update();

	if (input_->TriggerKey(DIK_SPACE)) {
		sceneNo = STAGE;
	}
}

void TitleScene::Draw()
{
	sphere_->Draw(camera_, uv);

	sprite_->Draw(monsterBall);
	sprite2_->Draw(uv);
}