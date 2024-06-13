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

class Mesh {
public:
	static Mesh* GetInsTance();

	void Initialize();

	void CreatePso();

	void Update();

	void Release();

private:
	DirectXCommon* dir_ = DirectXCommon::GetInsTance();
	TextureManager* texture_ = TextureManager::GetInstance();

	IDxcUtils* dxcUtils = nullptr;
	IDxcCompiler3* dxcCompiler = nullptr;
	IDxcIncludeHandler* includeHandler = nullptr;

	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;

	IDxcBlob* vertexShaderBlob;
	IDxcBlob* pixelShaderBlob;

	uint32_t index;

	static WinApp* window_;
};