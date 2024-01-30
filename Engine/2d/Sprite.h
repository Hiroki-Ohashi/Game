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
#include "Mesh.h"

class Sprite {
public:
	void Initialize(Vector2 pos, Vector2 scale, float index);

	void Update(float alpha);

	void Draw(uint32_t index);

	void Release();

	void CreateVertexResourceSprite(Vector2 pos, Vector2 scale);
	void CreateMaterialResourceSprite();
	void CreateTransformationMatrixResourceSprite();
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInbytes);

	void SetAlpha(float a) { a = materialDataSprite->color.z; }

	Material* GetMaterialDataSprite() { return materialDataSprite; }

private:
	DirectXCommon* dir_ = DirectXCommon::GetInsTance();
	TextureManager* texture_ = TextureManager::GetInstance();

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceSprite;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResourceSprite;
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResourceSprite;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResourceSprite;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};
	D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite{};

	VertexData* vertexDataSprite;
	Material* materialDataSprite;
	TransformationMatrix* transformationMatrixDataSprite;
	uint32_t* indexDataSprite;

	Transform transformSprite;
	Transform uvTransformSprite;

	bool isSprite = false;
};