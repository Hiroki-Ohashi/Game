#include "IScene.h"

/// <summary>
/// IScene.cpp
/// 現在Sceneのマネージャーのソースファイル
/// </summary>

IScene::~IScene() {}

int IScene::GetSceneNo()
{
	return sceneNo;
}

int IScene::sceneNo = OVER;