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

class Triangle {
public:

	// 初期化
	void Initialize(Vector4* pos);
	// 読み込み
	void Update();
	// 描画
	void Draw(Camera* camera, uint32_t index);
	// 解放
	void Release();

	void CreateVertexResource(Vector4* pos);
	void CreateMaterialResource();
	void CreateWVPResource();

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInbytes);

	Material* GetMaterialData() { return materialData; }
	bool GetTriangle() { return isTriangle; }
	void SetIsTriangle(bool isTriangle_) { isTriangle_ = isTriangle; }

private:
	TextureManager* texture_ = TextureManager::GetInstance();
	WorldTransform worldtransform_;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource;

	VertexData* vertexData;
	Material* materialData;
	TransformationMatrix* wvpData;

	EulerTransform transform;

	Matrix4x4 worldMatrix;

	bool isTriangle = false;

	static inline HRESULT hr_;
};