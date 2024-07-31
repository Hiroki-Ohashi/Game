#include "TitleScene.h"

TitleScene::~TitleScene()
{
}

void TitleScene::Initialize()
{
	textureManager_->Initialize();

	postProcess_ = std::make_unique<PostProcess>();
	postProcess_->Initialize(GRAY);
  
	title_ = std::make_unique<Sprite>();
	title_->Initialize(Vector2{ 0.0f, 0.0f }, Vector2{ 1280.0f, 720.0f }, 1.0f);

	title = textureManager_->Load("resources/start.png");
}


void TitleScene::Update()
{
	if (input_->TriggerKey(DIK_SPACE)) {
		sceneNo = STAGE;
	}
}

void TitleScene::Draw()
{
	title_->Draw(title);
}

void TitleScene::PostDraw()
{
	postProcess_->Draw();
}
