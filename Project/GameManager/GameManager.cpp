#include "GameManager.h"

GameManager::GameManager()
{
	winapp = WinApp::GetInsTance();
	winapp->Initialize(L"CG2");

	directX = DirectXCommon::GetInsTance();
	directX->Initialize();

	mesh = new Mesh;
	mesh->Initialize();

	input = Input::GetInsTance();
	input->Initialize();

	imgui =ImGuiManeger::GetInstance();
	imgui->Initialize();
}

GameManager::~GameManager()
{
	delete mesh;
}

GameManager* GameManager::GetInsTance()
{
	static GameManager instance;
	return &instance;
}

void GameManager::Run()
{
	CoInitializeEx(0, COINIT_MULTITHREADED);

	GameScene* gameScene = new GameScene;

	gameScene->Initialize();

	MSG msg{};

	// ウインドウの×ボタンが押されるまでループ
	while (msg.message != WM_QUIT) {
		// Windowにメッセージが来てたら最優先で処理させる
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			// ゲームの処理


			// 更新処理
			imgui->Update();
			directX->Update();
			mesh->Update();
			input->Update();
			gameScene->Update();

			// 描画処理
			gameScene->Draw();
			imgui->Draw();
			directX->Close();
		}
	}

	CoUninitialize();

	mesh->Release();
	directX->Release();
}
