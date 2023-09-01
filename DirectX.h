#pragma once

#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <cstdint>
#include "Function.h"


#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

class WinApp;

class DirectX {
public:
	void Initialize(WinApp* winApp_);
	void Fence();
	void Update();
	void Close();
	void Release(WinApp* winApp_);

	inline ID3D12Device* GetDevice() { return device_; }
	inline ID3D12GraphicsCommandList* GetCommandList() { return commandList_; }

private:
	IDXGIFactory7* dxgiFactory_ = nullptr;
	ID3D12Device* device_ = nullptr;
	IDXGIAdapter4* useAdapter_ = nullptr;
	ID3D12CommandQueue* commandQueue_ = nullptr;
	ID3D12CommandAllocator* commandAllocator_ = nullptr;
	ID3D12GraphicsCommandList* commandList_ = nullptr;
	IDXGISwapChain4* swapChain_ = nullptr;
	D3D12_RESOURCE_BARRIER barrier{};
	ID3D12DescriptorHeap* rtvDescriptorHeap_ = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
	ID3D12Resource* swapChainResources[2] = { nullptr };
	ID3D12Fence* fence = nullptr;
	uint64_t fenceValue = 0;
	HANDLE fenceEvent = 0;
	static inline HRESULT hr_;

	//static inline HWND hwnd = nullptr;
};
