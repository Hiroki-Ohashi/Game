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


/// <summary>
/// Triangle.h
/// 3Dの三角形生成のヘッダーファイル
/// </summary>
namespace Engine
{
	// Triangleクラス
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

		// Getter
		Material* GetMaterialData() { return materialData; }
		bool GetTriangle() { return isTriangle; }

		// Setter
		void SetIsTriangle(bool isTriangle_) { isTriangle_ = isTriangle; }

	private:
		// vertex生成
		void CreateVertexResource(Vector4* pos);
		// material作成
		void CreateMaterialResource();
		// wvp作成
		void CreateWVPResource();

		// Resource生成
		Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInbytes);

	private:
		// シングルトン呼び出し
		TextureManager* texture_ = TextureManager::GetInstance();

		// BufferView
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

		// Resource
		Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
		Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
		Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource;

		// Data
		VertexData* vertexData;
		Material* materialData;
		TransformationMatrix* wvpData;

		// Transform
		WorldTransform worldtransform_;
		EulerTransform transform;

		// Matrix
		Matrix4x4 worldMatrix;
		// 描画条件
		bool isTriangle = false;
	};
}
