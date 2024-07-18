#include "GameManager.h"

GameManager::GameManager()
{
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

	winapp = WinApp::GetInsTance();
	winapp->Initialize(L"CG2");

	directX = DirectXCommon::GetInsTance();
	directX->Initialize();

	mesh = new Mesh;
	mesh->Initialize();

	input = Input::GetInsTance();
	input->Initialize();

	imgui = ImGuiManeger::GetInstance();
	imgui->Initialize();

	// 各シーンの配列
	sceneArr_[TITLE] = std::make_unique<TitleScene>();
	sceneArr_[STAGE] = std::make_unique<GameScene>();
	sceneArr_[CLEAR] = std::make_unique<ClearScene>();

	currentSceneNo_ = TITLE;

	MSG msg{};

	sceneArr_[currentSceneNo_]->Initialize();

	// ウインドウの×ボタンが押されるまでループ
	while (msg.message != WM_QUIT) {
		// Windowにメッセージが来てたら最優先で処理させる
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			// ゲームの処理

			// シーンのチェック
			prvSceneNo_ = currentSceneNo_;
			currentSceneNo_ = sceneArr_[currentSceneNo_]->GetSceneNo();

			// シーン変更チェック
			if (prvSceneNo_ != currentSceneNo_) {
				sceneArr_[currentSceneNo_]->Initialize();
			}

			// 更新処理
			imgui->Update();
			directX->RenderTexture();
			mesh->Update();
			input->Update();
			sceneArr_[currentSceneNo_]->Update();

			// 描画処理
			sceneArr_[currentSceneNo_]->Draw();

			directX->SwapChain();
			sceneArr_[currentSceneNo_]->PostDraw();
			directX->RemoveBarrier();
			imgui->Draw();
		
			directX->Close();
		}
	}

	CoUninitialize();

	mesh->Release();
	directX->Release();
}
