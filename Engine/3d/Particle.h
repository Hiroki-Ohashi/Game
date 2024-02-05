#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <random>

#include "WinApp.h"
#include "Function.h"
#include "MathFunction.h"
#include "DirectXCommon.h"
#include "TextureManager.h"
#include "Camera.h"
#include "WorldTransform.h"

class Particles {
public:
	void Initialize(const std::string& filename, Vector3 pos);

	void Update();

	void Draw(Camera* camera, uint32_t index);

	void Release();

	void CreateVertexResource();
	void CreateMaterialResource();
	void CreateWVPResource();

	void CreatePso();

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInbytes);

	Particle MakeNewParticle(std::mt19937& randomEngine);

private:

	WinApp* winapp_ = WinApp::GetInsTance();
	TextureManager* texture_ = TextureManager::GetInstance();
	WorldTransform worldTransform_;

	ModelData modelData;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource;

	VertexData* vertexData;
	Material* materialData;
	TransformationMatrix* wvpData;

	Transform uvTransform;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;

	const static uint32_t kMaxInstance = 10;
	D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU_;
	D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU_;

	ParticleForGpu* instancingData_;

	Particle particles[kMaxInstance];


	bool isModel;

	CameraForGpu camera;

	const float kDeltaTime = 1.0f / 60.0f;
};