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

class Mesh {
public:
	void Initialize();

	void CreatePso();

	void Update();

	void Viewport();
	void Scissor();

	void Release();

private:
	DirectXCommon* dir_ = DirectXCommon::GetInsTance();

	IDxcUtils* dxcUtils = nullptr;
	IDxcCompiler3* dxcCompiler = nullptr;
	IDxcIncludeHandler* includeHandler = nullptr;

	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};

	IDxcBlob* vertexShaderBlob;
	IDxcBlob* pixelShaderBlob;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	D3D12_VERTEX_BUFFER_VIEW materialBufferView{};

	D3D12_ROOT_PARAMETER rootParameters[4] = {};

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};

	D3D12_VIEWPORT viewport{};
	D3D12_RECT scissorRect{};

	static inline HRESULT hr_;

	static WinApp* window_;
};