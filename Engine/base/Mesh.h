#pragma once

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
#include "TextureManager.h"

/// <summary>
/// Mesh.h
/// Mesh生成のソースファイル
/// </summary>
namespace Engine
{
	//Meshクラス
	class Mesh {
	public:
		// シングルトン
		static Mesh* GetInstance();
		// 初期化処理
		void Initialize();
		// PSO生成
		void CreatePso();
		// 更新処理
		void Update();
		// 解放処理
		void Release();

	private:
		// シングルトン
		DirectXCommon* dir_ = DirectXCommon::GetInstance();
		TextureManager* texture_ = TextureManager::GetInstance();

		// PSO
		IDxcUtils* dxcUtils = nullptr;
		IDxcCompiler3* dxcCompiler = nullptr;
		IDxcIncludeHandler* includeHandler = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;
		IDxcBlob* vertexShaderBlob;
		IDxcBlob* pixelShaderBlob;

		// テクスチャ
		uint32_t index;

		// ウインドウ
		static WinApp* window_;
	};
}