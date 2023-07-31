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
#include "DirectXCommon.h"
#include "Mesh.h"
#include "ImGuiManeger.h"
#include "MathFunction.h"
#include "externals/imgui/imgui.h"


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
		// 右下
		pos[i][2] = { -0.89f - (i * -0.07f),0.0f,0.0f,1.0f };
	}

	// 左下
	pos[0][0] = { -0.1f, 0.5f, 0.0f, 1.0f };
	// 上
	pos[0][1] = { 0.0f, 0.7f, 0.0f, 1.0f };
	// 右下
	pos[0][2] = { 0.1f, 0.5f, 0.0f, 1.0f };

	WinApp* winapp = new WinApp(L"CG2");
	DirectX* directX = new DirectX();
	Triangle* triangle[Max];
	ImGuiManeger* imgui = new ImGuiManeger;
	
	directX->Initialize(winapp);
	directX->Fence();

	MSG msg{};

	for (int i = 0; i < Max; i++) {
		triangle[i] = new Triangle;
		triangle[i]->DxcInitialize();
		triangle[i]->DxcPso(directX);
		triangle[i]->DxcVertexDraw(directX, pos[i]);
		triangle[i]->DxcViewport();
		triangle[i]->DxcScissor();
	}

	Transform transform{ {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
	Transform cameraTransform{ {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -5.0f} };

	imgui->Initialize(winapp, directX);

	// ウインドウの×ボタンが押されるまでループ
	while (msg.message != WM_QUIT) {
		// Windowにメッセージが来てたら最優先で処理させる
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			// ゲームの処理
			imgui->Update();
			directX->Update();


			transform.rotate.y += 0.03f;

			Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
			Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);

			Matrix4x4 viewMatrix = Inverse(cameraMatrix);
			Matrix4x4 projectionMatrix = MakePerspectiveMatrix(0.45f, float(winapp->kClientWidth) / float(winapp->kClientHeight), 0.1f, 100.0f);
			Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
	
			

			for (int i = 1; i < Max; i++) {
				*triangle[i]->wvpData = worldViewProjectionMatrix;
				triangle[i]->DxcUpdate(directX);
				triangle[0]->DxcUpdate(directX);
			}

			ImGui::Begin("Mesh Color");
			ImGui::ColorEdit3("Mesh Color", &triangle[0]->materialData->x);
			ImGui::End();

			ImGui::Begin("Mesh Position");
			ImGui::SliderFloat3("Mesh Pos", &transform.translate.x, -1.0f, 1.0f);
			ImGui::End();

			ImGui::Begin("Camera Position");
			ImGui::SliderFloat3("Camera Pos", &cameraTransform.translate.x, -1.0f, 1.0f);
			ImGui::SliderFloat3("Camera scale", &cameraTransform.scale.x, -1.0f, 1.0f);
			ImGui::SliderFloat3("Camera rotate", &cameraTransform.rotate.x, -1.0f, 1.0f);
			ImGui::End();

			imgui->Draw(directX);

			directX->Close();
		}
	}


	for (int i = 0; i < Max; i++) {
		triangle[i]->DxcRelease();
	}
	imgui->Release();
	directX->Release(winapp);
	
	

	return 0;
}