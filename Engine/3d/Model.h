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

/// <summary>
/// Model.h
/// 3Dのオブジェクト生成のヘッダーファイル
/// </summary>

// Modelクラス
class Model {
public:
	// デストラクタ
	~Model();

	// 初期化処理
	void Initialize(const std::string& filename, EulerTransform transform);
	// 更新処理
	void Update();
	// 描画処理
	void Draw(Camera* camera, uint32_t index);

	// Getter
	ModelData GetModelData() { return modelData; }

	// Setter
	void SetWorldTransform(WorldTransform worldtransform) { worldTransform_ = worldtransform; }
	void SetPosition(Vector3 pos) { worldTransform_.translate = pos; }
	void SetRotation(Vector3 rot) { worldTransform_.rotate = rot; }
	void SetScale(Vector3 scale) { worldTransform_.scale = scale; }
	void SetLight(bool isLight) { materialData->enableLighting = isLight; }

private:
	// vertex生成
	void CreateVertexResource();
	// material作成
	void CreateMaterialResource();
	// wvp作成
	void CreateWVPResource();
	// PSO作成
	void CreatePso();

	// Resource生成
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInbytes);

private:
	// シングルトン呼び出し
	WinApp* winapp_ = WinApp::GetInsTance();
	TextureManager* texture_ = TextureManager::GetInstance();
	Light* light_ = Light::GetInstance();

	// Transform
	WorldTransform worldTransform_;
	EulerTransform transform;
	EulerTransform uvTransform;

	// model
	ModelData modelData;
	uint32_t* mappedIndex;

	// BufferView
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};

	// Resource
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource;

	// Data
	VertexData* vertexData;
	Material* materialData;
	TransformationMatrix* wvpData;

	// 描画条件
	bool isModel;

	// camera
	CameraForGpu camera_;

	// PSO
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;

};
