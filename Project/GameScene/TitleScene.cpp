#include "TitleScene.h"

TitleScene::~TitleScene()
{
}

void TitleScene::Initialize()
{
	textureManager_->Initialize();
}


void TitleScene::Update()
{

	if (input_->TriggerKey(DIK_SPACE)) {
		sceneNo = STAGE;
	}
}

void TitleScene::Draw()
{
}

void TitleScene::PostDraw()
{
}
