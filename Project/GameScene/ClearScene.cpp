#include "ClearScene.h"

ClearScene::~ClearScene()
{
}

void ClearScene::Initialize()
{
	textureManager_->Initialize();

	postProcess_ = std::make_unique<PostProcess>();
	postProcess_->Initialize(NONE);

	title_ = std::make_unique<Sprite>();
	title_->Initialize(Vector2{ 0.0f, 0.0f }, Vector2{ 1280.0f, 720.0f }, 1.0f);

	title = textureManager_->Load("resources/clear.png");
}

void ClearScene::Update()
{
	if (input_->TriggerKey(DIK_SPACE)) {
		sceneNo = TITLE;
	}
}

void ClearScene::Draw()
{
	title_->Draw(title);
}

void ClearScene::PostDraw()
{
	postProcess_->Draw();
}
