#include "SrvManager.h"

namespace Engine {

	SrvManager* SrvManager::GetInstance()
	{
		static SrvManager instance;
		return &instance;
	}

	void Engine::SrvManager::Initialize()
	{
		// descriptorHeapの生成
		descriptorHeap = DirectXCommon::GetInstance()->CreateDescriptorHeap(DirectXCommon::GetInstance()->GetDevice().Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);
		// deisptorHeapSize1個分を取得
		descriptorSize = DirectXCommon::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	void SrvManager::CreateSRVforTexture2D(const TextureData& textureData)
	{
		// SRV 設定
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = textureData.mataData.format;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		if (textureData.mataData.IsCubemap()) {
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
			srvDesc.TextureCube.MostDetailedMip = 0;
			srvDesc.TextureCube.MipLevels = UINT_MAX;
			srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
		}
		else {
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = UINT(textureData.mataData.mipLevels);
		}

		DirectXCommon::GetInstance()->GetDevice()->CreateShaderResourceView(textureData.resource.Get(), &srvDesc, GetCPUDescriptorHandle(textureData.srvIndex));
	}

	void SrvManager::CreateSRVforStructuredBuffer(uint32_t srvIndex, Microsoft::WRL::ComPtr<ID3D12Resource> pResource, UINT numElements, UINT structureByteStride)
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc{};
		instancingSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
		instancingSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		instancingSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		instancingSrvDesc.Buffer.FirstElement = 0;
		instancingSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		instancingSrvDesc.Buffer.NumElements = numElements;
		instancingSrvDesc.Buffer.StructureByteStride = structureByteStride;

		// SRVの生成
		DirectXCommon::GetInstance()->GetDevice()->CreateShaderResourceView(pResource.Get(), &instancingSrvDesc, GetCPUDescriptorHandle(srvIndex));
	}

	//void SrvManager::CreateSRVRenderTexture(uint32_t srvIndex, Microsoft::WRL::ComPtr<ID3D12Resource> pResource, DXGI_FORMAT Format, UINT MipLevels)
	//{
	//	D3D12_SHADER_RESOURCE_VIEW_DESC renderTextureSrvDesc{};
	//	renderTextureSrvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//	renderTextureSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	//	renderTextureSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	//	renderTextureSrvDesc.Texture2D.MipLevels = 1;

	//	// SRVの作成
	//	dir_->GetDevice()->CreateShaderResourceView(pResource.Get(), &renderTextureSrvDesc, srvHandle);
	//}

	void SrvManager::PreDraw()
	{
		// 描画用のDescriptorHeapの設定
		ID3D12DescriptorHeap* descriptorHeaps[] = { descriptorHeap.Get() };
		DirectXCommon::GetInstance()->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);
	}

	uint32_t SrvManager::Allocate()
	{
		// returnする番号を記録
		int index = useIndex;
		// 次回のために1進める
		useIndex++;
		// 上で記録した番号をreturn
		return index;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE SrvManager::GetCPUDescriptorHandle(uint32_t index)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
		handleCPU.ptr += (descriptorSize * index);
		return handleCPU;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE SrvManager::GetGPUDescriptorHandle(uint32_t index)
	{
		D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
		handleGPU.ptr += (descriptorSize * index);
		return handleGPU;
	}

	void SrvManager::SetGraphicsRootDescriptorTable(UINT RootParameterIndex, uint32_t srvIndex)
	{
		DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootDescriptorTable(RootParameterIndex, GetGPUDescriptorHandle(srvIndex));
	}
}