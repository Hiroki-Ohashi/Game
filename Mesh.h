#pragma once
#include <Windows.h>
#include <cstdint>
#include "Function.h"

class WinApp;
class DirectX;

class Mesh {
public:
	void Initialize(DirectX* dir_);

	void Pso(DirectX* dir_);
	void Viewport();
	void Scissor();

	void Update(DirectX* dir_);

	void Release();


	IDxcUtils* dxcUtils = nullptr;
	IDxcCompiler3* dxcCompiler = nullptr;
	IDxcIncludeHandler* includeHandler = nullptr;


	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	ID3D12RootSignature* rootSignature = nullptr;
	ID3D12PipelineState* graphicsPipelineState = nullptr;

	IDxcBlob* vertexShaderBlob;
	IDxcBlob* pixelShaderBlob;

	D3D12_VIEWPORT viewport{};
	D3D12_RECT scissorRect{};

	static inline HRESULT hr_;

	static WinApp* window_;
};