#include "IScene.h"

IScene::~IScene() {}

int IScene::GetSceneNo()
{
	return sceneNo;
}

int IScene::sceneNo = TITLE;