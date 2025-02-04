#pragma once
#include <Windows.h>
#include <cstdint>
#include <string>
#include <format>
#include <cassert>
#include <vector>
#include <fstream>
#include <sstream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Function.h"
#include "MathFunction.h"
#include "DirectXCommon.h"
#include "d3dx12.h"
#include <unordered_map>
#include <mutex>

namespace Engine
{
	// TextureManagerクラス
	class TextureManager {
	public:
		// シングルトン
		static TextureManager* GetInstance();
		// 初期化
		void Initialize();
		// 読み込み
		uint32_t Load(const std::string& filePath);

		// アニメーション読み込み
		Animation LoadAnimation(const std::string& directoryPath, const std::string& filename);

		// テクスチャセット
		void SetTexture(const std::string& filePath, uint32_t index);

		// テクスチャ読み込み
		DirectX::ScratchImage LoadTexture(const std::string& filePath);

		// Getter
		const D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSRVHandleGPU(uint32_t index) { return textureSrvHandleGPU[index]; }
		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);
		uint32_t GetDiscreptorSize() { return descriptorSizeSRV; }
		Microsoft::WRL::ComPtr<ID3D12Resource> GetTextureResource(uint32_t index) { return textureResource[index].Get(); }

		// リソース作成
		Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, const DirectX::TexMetadata& metadata);
		Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInbytes);

		// 読み込み関数
		ModelData LoadModelFile(const std::string& directoryPath, const std::string& filename);
		MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);
		ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);
		Animation LoadAnimationFile(const std::string& directoryPath, const std::string& filename);

		// ノード読み込み
		Node ReadNode(aiNode* node);

		//uint32_t GetDiscrepterRtvSize() { return descriptorSizeRTV; }

		[[nodiscard]]
		Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);

	private:
		DirectXCommon* dir_ = DirectXCommon::GetInsTance();

		// DescriptorSizeを取得しておく
		uint32_t descriptorSizeRTV;
		uint32_t descriptorSizeDSV;
		uint32_t descriptorSizeSRV;

		static const int kMaxTexture = 1000;
		int32_t textureIndex_;

		Microsoft::WRL::ComPtr< ID3D12Resource> intermediateResource[kMaxTexture];
		Microsoft::WRL::ComPtr<ID3D12Resource> textureResource[kMaxTexture];

		D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU[kMaxTexture];
		D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU[kMaxTexture];

		static const int kMaxAnimation = 100;
		uint32_t animationIndex_;

		Animation animation[kMaxAnimation];

		// キャッシュ用データ構造
		std::unordered_map<std::string, ModelData> modelCache;
		std::mutex cacheMutex; // スレッドセーフ性を確保するためのミューテックス

		// テクスチャキャッシュ：キーはファイルパス、値はリソース情報
		std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12Resource>> textureCache;
		std::unordered_map<std::string, D3D12_SHADER_RESOURCE_VIEW_DESC> srvCache;

		std::mutex TextureCacheMutex; // キャッシュ用ミューテックス
	};
}