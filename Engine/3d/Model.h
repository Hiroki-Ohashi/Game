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
#include "Camera.h"

class Model {
public:
	void Initialize(const std::string& filename, Transform transform);

	void Update();

	void Draw(Camera* camera, uint32_t index);

	void Release();

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInbytes);

private:

	void CreateVertexResource();
	void CreateMaterialResource();
	void CreateWVPResource();
	void CreateDirectionalResource();

private:

	WinApp* winapp_ = WinApp::GetInsTance();
	TextureManager* texture_ = TextureManager::GetInstance();

	WorldTransform worldTransform_;

	ModelData modelData;
	Animation animation;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource;

	VertexData* vertexData;
	Material* materialData;
	TransformationMatrix* wvpData;
	DirectionalLight directionalLightData;

	Transform transform;
	Transform uvTransform;

	bool isModel;

	CameraForGpu camera;

};
