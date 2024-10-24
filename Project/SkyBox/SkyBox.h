#pragma once
#include <Camera.h>
#include <TextureManager.h>
#include <WorldTransform.h>

/// <summary>
/// SkyBox.h
/// SkyBox生成のヘッダーファイル
/// </summary>

class SkyBox {
public:
	// 初期化処理
	void Initialize();
	// 描画処理
	void Draw(Camera* camera, uint32_t index);
private:
	// PSO作成
	void CreatePSO();
	// vertex生成
	void CreateVertexResource();
	// index生成
	void createIndexResource();
	// material作成
	void CreateMaterialResource();
	// wvp作成
	void CreateWVP();
	// Resource生成
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInbytes);
private:
	// シングルトン呼び出し
	DirectXCommon* dir_ = DirectXCommon::GetInsTance();
	TextureManager* texture_ = TextureManager::GetInstance();

	// Resource
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource;

	// BufferView
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};

	// Data
	VertexData* vertexData;
	Material* materialData;
	TransformationMatrix* wvpResourceData;

	// Transform
	WorldTransform worldTransform_;
	EulerTransform transform;
	EulerTransform uvTransform;
	TransformationMatrix* worldMatrix;

	// 頂点データ
	uint32_t vertexIndex = 4 * 6;
	uint32_t* indexData;

	// PSO
	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;
	IDxcBlob* vertexShaderBlob;
	IDxcBlob* pixelShaderBlob;

	// camera
	CameraForGpu camera;
};