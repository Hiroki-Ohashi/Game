#pragma once
#include "WinApp.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "GameScene.h"
#include "ImGuiManeger.h"
#include "imgui.h"


class GameManager {
public:
	GameManager();
	~GameManager();

	static GameManager* GetInsTance();

	void Run();

private:
	WinApp* winapp = nullptr;
	DirectXCommon* directX = nullptr;
	Mesh* mesh = nullptr;
	Input* input = nullptr;
	ImGuiManeger* imgui = nullptr;
};