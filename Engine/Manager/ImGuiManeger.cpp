#include "ImGuiManeger.h"
#include "DirectXCommon.h"
#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"

/// <summary>
/// ImGuiManager.cpp
/// ImGui生成のソースファイル
/// </summary>

ImGuiManeger* ImGuiManeger::GetInstance()
{
	static ImGuiManeger instance;
	return &instance;
}

void ImGuiManeger::Initialize(){
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(WinApp::GetInsTance()->GetHwnd());
	ImGui_ImplDX12_Init(DirectXCommon::GetInsTance()->GetDevice().Get(),
		DirectXCommon::GetInsTance()->GetSwapChainDesc().BufferCount,
		DirectXCommon::GetInsTance()->GetRtvDesc().Format,
		DirectXCommon::GetInsTance()->GetSrvDescriptorHeap().Get(),
		DirectXCommon::GetInsTance()->GetSrvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
		DirectXCommon::GetInsTance()->GetSrvDescriptorHeap()->GetGPUDescriptorHandleForHeapStart()
	);
}

void ImGuiManeger::Update(){
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImGuiManeger::Draw(){
	// 開発用UIの処理。実際に開発用のUIを出す場合はココをゲーム固有の処理に置き換える
	//ImGui::ShowDemoWindow();
	// ImGuiの内部コマンドを生成する
	ImGui::Render();

#ifdef _DEBUG
	// 実際にcommandListのImGuiの描画コマンドを積む
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), DirectXCommon::GetInsTance()->GetCommandList().Get());
#endif // DEBUG
}

void ImGuiManeger::Release(){
	// Cleanup
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
