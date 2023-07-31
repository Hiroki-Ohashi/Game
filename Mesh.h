#pragma once
#include <Windows.h>
#include <cstdint>
#include "Function.h"
#include "MathFunction.h"

class WinApp;
class DirectX;

class Triangle {
public:
	Triangle();
	~Triangle();

	void DxcInitialize();

	void DxcPso(DirectX* dir_);

	void DxcVertexDraw(DirectX* dir_, Vector4* pos);
	void DxcUpdate(DirectX* dir_);

	void DxcViewport();
	void DxcScissor();

	void DxcRelease();

	ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInbytes);

	IDxcUtils* dxcUtils = nullptr;
	IDxcCompiler3* dxcCompiler = nullptr;
	IDxcIncludeHandler* includeHandler = nullptr;


	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	ID3D12RootSignature* rootSignature = nullptr;
	ID3D12PipelineState* graphicsPipelineState = nullptr;

	D3D12_HEAP_PROPERTIES uploadHeapProperties{};

	ID3D12Resource* vertexResource;
	ID3D12Resource* materialResource;
	ID3D12Resource* wvpResource;

	Vector4* vertexData;
	Vector4* materialData;
	Matrix4x4* wvpData;

	IDxcBlob* vertexShaderBlob;
	IDxcBlob* pixelShaderBlob;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	D3D12_VERTEX_BUFFER_VIEW materialBufferView{};

	D3D12_VIEWPORT viewport{};
	D3D12_RECT scissorRect{};

	static inline HRESULT hr_;

	static WinApp* window_;
};