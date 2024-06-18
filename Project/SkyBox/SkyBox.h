#pragma once
#include <Camera.h>
#include <TextureManager.h>
#include <WorldTransform.h>


class SkyBox {
public:
	void Initialize();
	void Draw(Camera* camera, uint32_t index);
private:
	void CreatePSO();
	void CreateVertexResource();
	void CreateMaterialResource();
	void CreateWVP();
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInbytes);
private:
	DirectXCommon* dir_ = DirectXCommon::GetInsTance();
	TextureManager* texture_ = TextureManager::GetInstance();
	WorldTransform worldTransform_;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

	VertexData* vertexData;
	Material* materialData;
	TransformationMatrix* wvpResourceData;

	EulerTransform transform;
	EulerTransform uvTransform;
	TransformationMatrix* worldMatrix;

	uint32_t vertexIndex = 6 * 6;

	IDxcUtils* dxcUtils = nullptr;
	IDxcCompiler3* dxcCompiler = nullptr;
	IDxcIncludeHandler* includeHandler = nullptr;

	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;

	IDxcBlob* vertexShaderBlob;
	IDxcBlob* pixelShaderBlob;
};