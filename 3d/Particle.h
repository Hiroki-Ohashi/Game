#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <sstream>
#include "WinApp.h"
#include "Function.h"
#include "MathFunction.h"
#include "DirectXCommon.h"
#include "TextureManager.h"
#include "Camera.h"
#include "WorldTransform.h"

class Particle {
public:
	void Initialize();

	void Update();

	void Draw(Camera* camera, uint32_t index);

	void Release();

	void CreateVertexResource();
	void CreateMaterialResource();
	void CreateWVPResource();

	void CreatePso();

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInbytes);

private:

	WinApp* winapp_ = WinApp::GetInsTance();
	TextureManager* texture_ = TextureManager::GetInstance();
	WorldTransform worldTransform_;

	ModelData modelData;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource;

	VertexData* vertexData;
	Material* materialData;
	TransformationMatrix* wvpData;

	Transform transform;
	Transform uvTransform;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;

	const static uint32_t kNumInstance = 10;
	D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU_;
	D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU_;
	TransformationMatrix* instancingData_;
	Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource_;
	Transform transform_[kNumInstance];


	bool isModel;
};
