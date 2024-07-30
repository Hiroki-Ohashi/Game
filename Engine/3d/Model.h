#pragma once

#include <Windows.h>
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
#include "Mesh.h"
#include "Camera.h"
#include "WorldTransform.h"
#include "Light.h"

class Model {
public:
	void Initialize(const std::string& filename, EulerTransform transform);

	void Update();

	void Draw(Camera* camera, uint32_t index);

	ModelData GetModelData() { return modelData; }
	void SetWorldTransform(WorldTransform worldtransform) { worldTransform_ = worldtransform; }

	void SetPosition(Vector3 pos) { worldTransform_.translate = pos; }
	void SetRotation(Vector3 rot) { worldTransform_.rotate = rot; }
	void SetScale(Vector3 scale) { worldTransform_.scale = scale; }

private:

	void CreateVertexResource();
	void CreateMaterialResource();
	void CreateWVPResource();
	void CreatePso();

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInbytes);

private:

	WinApp* winapp_ = WinApp::GetInsTance();
	TextureManager* texture_ = TextureManager::GetInstance();
	Light* light_ = Light::GetInstance();

	WorldTransform worldTransform_;

	ModelData modelData;
	uint32_t* mappedIndex;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource;

	VertexData* vertexData;
	Material* materialData;
	TransformationMatrix* wvpData;

	EulerTransform transform;
	EulerTransform uvTransform;

	bool isModel;

	CameraForGpu camera_;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;

};
