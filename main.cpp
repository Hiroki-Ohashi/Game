#include <Windows.h>
#include <cstdint>
#include <string>
#include <format>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <dxgidebug.h>
#include <dxcapi.h>
#include "WinApp.h"
#include "Function.h"
#include "DirectX.h"
#include "Triangle.h"
#include "Mesh.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")



// WIndowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// 三角形の数
	const int Max = 30;

	Vector4 pos[Max][3];
	for (int i = 0; i < Max; i++) {
		// 左下
		pos[i][0] = { -0.95f - (i * -0.07f),0.0f,0.0f,1.0f };
		// 上
		pos[i][1] = { -0.92f - (i * -0.07f),0.05f,0.0f,1.0f };
		// 右上
		pos[i][2] = { -0.89f - (i * -0.07f),0.0f,0.0f,1.0f };
	}

	WinApp* winapp = new WinApp(L"CG2");
	DirectX* directX = new DirectX();
	Triangle* triangle[Max];
	Mesh* mesh = new Mesh();
	
	directX->Initialize(winapp);
	mesh->Initialize(directX);

	MSG msg{};

	for (int i = 0; i < Max; i++) {
		triangle[i] = new Triangle;
		triangle[i]->Initialize(directX, pos[i]);
	}
	

	// ウインドウの×ボタンが押されるまでループ
	while (msg.message != WM_QUIT) {
		// Windowにメッセージが来てたら最優先で処理させる
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			// ゲームの処理
			directX->Update();
			mesh->Update(directX);

			for (int i = 0; i < Max; i++) {
				triangle[i]->Draw(directX);
			}

			directX->Close();
		}
	}

	for (int i = 0; i < Max; i++) {
		triangle[i]->Release();
		delete triangle[i];
	}

	mesh->Release();
	delete mesh;
	directX->Release(winapp);
	delete directX;
	

	return 0;
}