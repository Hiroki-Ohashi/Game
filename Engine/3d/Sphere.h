#pragma once
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <cstdint>

#include "WinApp.h"
#include "Function.h"
#include "MathFunction.h"
#include "DirectXCommon.h"
#include "TextureManager.h"
#include "Camera.h"
#include "WorldTransform.h"
#include "Light.h"

/// <summary>
/// Sphere.h
/// 球体生成のヘッダーファイル
/// </summary>

class Sphere {
public:
	// デストラクタ
	~Sphere();

	// 初期化処理
	void Initialize();
	// 更新処理
	void Update();
	// 描画処理
	void Draw(Camera* camera, uint32_t index);
	// 解放処理
	void Release();

	// Getter
	Material* GetMaterialDataSphere() { return materialDataSphere; }
private:
	// vertex生成
	void CreateVertexResourceSphere();
	// material作成
	void CreateMaterialResourceSphere();
	// wvp作成
	void CreateTransformationMatrixResourceSphere();
	// Light作成
	void CreateDirectionalResource();

	// Resource生成
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInbytes);

private:
	// シングルトン呼び出し
	Light* light_ = Light::GetInstance();
	TextureManager* texture_ = TextureManager::GetInstance();

	// Resource
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceSphere;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResourceSphere;
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResourceSphere;
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource;

	// BufferView
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSphere{};

	// Data
	VertexData* vertexDataSphere;
	Material* materialDataSphere;
	TransformationMatrix* wvpResourceDataSphere;
	DirectionalLight* directionalLightData;

	// Transform
	WorldTransform worldTransform_;
	EulerTransform transformSphere;
	EulerTransform uvTransformSphere;
	TransformationMatrix* worldMatrixSphere;

	// 球面の分割
	const uint32_t kSubdivision = 128; //分割数
	uint32_t vertexIndex = (kSubdivision * kSubdivision) * 6;

	// camera
	CameraForGpu camera_;
};