#include "TitleScene.h"

TitleScene::~TitleScene()
{
}

void TitleScene::Initialize()
{
	textureManager_->Initialize();

	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize(Vector2{ 0.0f, 0.0f }, Vector2{ 1280.0f, 720.0f });

	postProcess_ = std::make_unique<PostProcess>();
	postProcess_->Initialize();

	title = textureManager_->Load("resources/black.png");
}


void TitleScene::Update()
{
	if (input_->TriggerKey(DIK_SPACE)) {
		sceneNo = STAGE;
	}
}

void TitleScene::Draw()
{
	sprite_->Draw(title);
}

void TitleScene::PostDraw()
{
	postProcess_->Draw();
}
