#pragma once
#include "DirectXCommon.h"

namespace Engine
{
	class SrvManager {
	public:
		// シングルトン
		static SrvManager* GetInstance();
		// 初期化
		void Initialize();
		// SRV生成(テクスチャ用)
		void CreateSRVforTexture2D(const TextureData& textureData);
		// SRV生成(Structured Buffer用)
		void CreateSRVforStructuredBuffer(uint32_t srvIndex, Microsoft::WRL::ComPtr<ID3D12Resource> pResource, UINT numElements, UINT structureByteStride);
		// SRV生成(RenderTexture用)
		void CreateSRVRenderTexture(uint32_t srvIndex, Microsoft::WRL::ComPtr<ID3D12Resource> pResource);
		// SRV生成(Depth用)
		void CreateSRVDepthTexture(uint32_t srvIndex, Microsoft::WRL::ComPtr<ID3D12Resource> pResource);

		// 描画
		void PreDraw();
		void PostDraw();

		uint32_t Allocate();

		// Getter
		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetSrvDescriptorHeap() { return descriptorHeapPostEffect.Get(); }
		D3D12_CPU_DESCRIPTOR_HANDLE GetEffectCPUDescriptorHandle(uint32_t index);
		D3D12_GPU_DESCRIPTOR_HANDLE GetEffectGPUDescriptorHandle(uint32_t index);
		uint32_t GetSize() { return descriptorSize; }

		// Setter
		void SetGraphicsRootDescriptorTable(UINT RootParameterIndex, uint32_t srvIndex);
		void SetGraphicsRootDescriptorTableTypeEffect(UINT RootParameterIndex, uint32_t srvIndex);

	private:
		// 最大SRV数
		static const uint32_t kMaxSRVCount = 512;
		// ディスクリプタサイズ
		uint32_t descriptorSize;
		//  ディスクリプタヒープ
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeapPostEffect;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeapImgui;
		//  ディスクリプタハンドル
		D3D12_CPU_DESCRIPTOR_HANDLE srvCpuHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;
		// 次に使うSRVインデックス
		uint32_t useIndex = 1;
	private:
		SrvManager() = default;
		~SrvManager() = default;
		SrvManager(SrvManager&) = delete;
		SrvManager& operator = (SrvManager&) = delete;
	};
}