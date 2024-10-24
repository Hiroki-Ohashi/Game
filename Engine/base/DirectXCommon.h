#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <cassert>
#include <string>
#include <format>
#include <cstdint>
#include <wrl.h>
#include <Windows.h>
#include <chrono>
#include <thread>

#include "WinApp.h"
#include "Function.h"
#include "MathFunction.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

/// <summary>
/// DirectXCommon.h
/// DirectXのヘッダーファイル
/// </summary>

// DirectXCommonクラス
class DirectXCommon {
public:
	// シングルトン化
	static DirectXCommon* GetInsTance();
	// デストラクタ
	~DirectXCommon();

	// Default Methods
	void Initialize();
	void RenderTexture();
	// void Draw(); //not use
	void Release();
	
	// Accessor
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GetCommandList() { return commandList_.Get(); }
	Microsoft::WRL::ComPtr<ID3D12Device> GetDevice() { return device_.Get(); }
	DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc() { return swapChainDesc; }
	D3D12_RENDER_TARGET_VIEW_DESC GetRtvDesc() { return rtvDesc; }
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetSrvDescriptorHeap() { return srvDescriptorHeap_.Get(); }
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetSrvDescriptorHeap2() { return srvDescriptorHeap2_.Get(); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGpu() { return srvGpuHandle; }
	D3D12_RESOURCE_BARRIER GetBarrier() const { return barrier; }

	void SetBarrier(D3D12_RESOURCE_BARRIER barrier_) { barrier_ = barrier; }

	// User Methods
	void Fence();
	void SwapChain();
	void RemoveBarrier();
	void Close();

	void Viewport();
	void Scissor();

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(Microsoft::WRL::ComPtr<ID3D12Device> device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, int32_t width, int32_t height);
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateRenderTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, int32_t width, int32_t height, DXGI_FORMAT format, const Vector4& clearColor);

private:

	// FPS固定初期化
	void InitializeFixFPS();
	// FPS固定更新
	void UpdateFixFPS();

	// DirectX生成変数
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Device> device_ = nullptr;
	Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_ = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};

	// barriar
	D3D12_RESOURCE_BARRIER barrier{};
	D3D12_RESOURCE_BARRIER barrier2{};

	// DescripterHeap
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap2_ = nullptr;

	// DescripterHandle
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};

	// Resource
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources[2] = { nullptr };
	Microsoft::WRL::ComPtr<ID3D12Resource> renderTextureResource;

	// fence
	Microsoft::WRL::ComPtr<ID3D12Fence> fence = nullptr;
	uint64_t fenceValue = 0;
	HANDLE fenceEvent = 0;
	std::chrono::steady_clock::time_point reference_;

	const Vector4 kRenderTargetClearValue = { 0.1f, 0.25f, 0.5f, 1.0f }; // いったんわかりやすいように赤

	D3D12_VIEWPORT viewport{};
	D3D12_RECT scissorRect{};

	UINT backBufferIndex;

	static inline HRESULT hr_;
};
