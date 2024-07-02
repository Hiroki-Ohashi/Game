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

	void Update();

	void Draw(uint32_t index);

	void Release();

	void CreateVertexResourceSprite(Vector2 pos, Vector2 scale);
	void CreateMaterialResourceSprite();
	void CreateTransformationMatrixResourceSprite();
	void CreatePso();

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInbytes);


	Material* GetMaterialDataSprite() { return materialDataSprite; }

	const Vector2& GetAnchorPoint() const { return anchorPoint; }
	// 画像の切り出しサイズ
	Vector2 GetTextureSize() { return textureSize; }

	Vector2 GetTextureLeftTop() { return textureLeftTop; }

	void SetAnchorPoint(const Vector2 anchorPoint_) { this->anchorPoint = anchorPoint_; }

	void SetSize(Vector2 size) { textureSize = size; }

	void SetTextureLeftTop(const Vector2 textureLeftTop_) { this->textureLeftTop = textureLeftTop_; }

	// テクスチャのサイズをスプライトに合わせる
	void AdjustTextureSize();

private:
	DirectXCommon* dir_ = DirectXCommon::GetInsTance();
	TextureManager* texture_ = TextureManager::GetInstance();

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceSprite;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResourceSprite;
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResourceSprite;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResourceSprite;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};
	D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite{};

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;

	VertexData* vertexDataSprite;
	Material* materialDataSprite;
	TransformationMatrix* transformationMatrixDataSprite;
	uint32_t* indexDataSprite;

	EulerTransform transformSprite;
	EulerTransform uvTransformSprite;

	bool isSprite = false;

	uint32_t textureIndex;

	Vector2 anchorPoint = { 0.0f, 0.0f };

	// 画像の左上の座標
	Vector2 textureLeftTop = { 0.0f,0.0f };
	// テクスチャの切り出しサイズ
	Vector2 textureSize = { 100.0f, 100.0f };

};