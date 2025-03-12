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
#include "SrvManager.h"
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
		void SetTexture(const std::string& filePath);

		// テクスチャ読み込み
		DirectX::ScratchImage LoadTexture(const std::string& filePath);

		// Getter
		uint32_t GetSrvIndex(const std::string& filename) { return textureDatas[filename].srvIndex; }
		const D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSRVHandleGPU(const std::string& filename) { return textureDatas[filename].srvHandleGPU; }
		Microsoft::WRL::ComPtr<ID3D12Resource> GetTextureResource(const std::string& filename) { return textureDatas[filename].resource; }

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

		[[nodiscard]]
		Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);

	private:
		DirectXCommon* dir_ = DirectXCommon::GetInstance();
		SrvManager* srvManager_ = SrvManager::GetInstance();

		static const int kMaxTexture = 1000;
		Microsoft::WRL::ComPtr< ID3D12Resource> intermediateResource[kMaxTexture];

		static const int kMaxAnimation = 100;
		uint32_t animationIndex_;

		Animation animation[kMaxAnimation];

		// TextureData
		std::unordered_map<std::string, TextureData> textureDatas;
		
		// キャッシュ用データ構造
		std::unordered_map<std::string, ModelData> modelCache;
		std::mutex cacheMutex;

	private:
		TextureManager() = default;
		~TextureManager() = default;
		TextureManager(TextureManager&) = delete;
		TextureManager& operator = (TextureManager&) = delete;
	};
}