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

class TextureManager {
public:

	static TextureManager* GetInstance();

	void Initialize();

	uint32_t Load(const std::string& filePath);

	Animation LoadAnimation(const std::string& directoryPath, const std::string& filename);

	void SetTexture(const std::string& filePath, uint32_t index);
	
	DirectX::ScratchImage LoadTexture(const std::string& filePath);

	const D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSRVHandleGPU(uint32_t index) { return textureSrvHandleGPU[index]; }

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);
	uint32_t GetDiscreptorSize() { return descriptorSizeSRV; }
	
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, const DirectX::TexMetadata& metadata);
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInbytes);

	ModelData LoadModelFile(const std::string& directoryPath, const std::string& filename);
	MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);
	Animation LoadAnimationFile(const std::string& directoryPath, const std::string& filename);

	Node ReadNode(aiNode* node);

	Microsoft::WRL::ComPtr<ID3D12Resource> GetTextureResource(uint32_t index) { return textureResource[index].Get(); }

	//uint32_t GetDiscrepterRtvSize() { return descriptorSizeRTV; }

	[[nodiscard]]
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);

private:
	DirectXCommon* dir_ = DirectXCommon::GetInsTance();

	static const int kMaxTexture = 100;
	uint32_t textureIndex_;

	// DescriptorSizeを取得しておく
	uint32_t descriptorSizeRTV;
	uint32_t descriptorSizeDSV;
	uint32_t descriptorSizeSRV;

	Microsoft::WRL::ComPtr< ID3D12Resource> intermediateResource[kMaxTexture];
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource[kMaxTexture];

	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU[kMaxTexture];
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU[kMaxTexture];

	static const int kMaxAnimation = 100;
	uint32_t animationIndex_;

	Animation animation[kMaxAnimation];
};