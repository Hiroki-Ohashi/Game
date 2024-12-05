#include "DirectXCommon.h"

/// <summary>
/// DirectXCommon.cpp
/// DirectXのソースファイル
/// </summary>

namespace Engine
{
	DirectXCommon* DirectXCommon::GetInsTance()
	{
		static DirectXCommon instance;
		return &instance;
	}

	DirectXCommon::~DirectXCommon()
	{
	}

	void DirectXCommon::Initialize() {

		// FPS固定初期化
		InitializeFixFPS();

		// DXGIファクトリーの生成
		hr_ = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
		assert(SUCCEEDED(hr_));

		// ダイレクトXとデバイスをつなぐアダプタ
		// いい順にアダプタを頼む
		for (UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i,
			DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter_)) !=
			DXGI_ERROR_NOT_FOUND; ++i) {
			// アダプタの情報を取得
			DXGI_ADAPTER_DESC3 adapterDesc{};
			hr_ = useAdapter_->GetDesc3(&adapterDesc);
			assert(SUCCEEDED(hr_));
			// ソフトウェアアダプタでなければ採用
			if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
				// 採用したアダプタの情報をログに出力。
				Convert::Log(Convert::ConvertString(std::format(L"Use Adapater:{}\n", adapterDesc.Description)));
				break;
			}
			useAdapter_ = nullptr;
		}
		// 適切なアダプタが見つからなかったら起動できない
		assert(useAdapter_ != nullptr);

		D3D_FEATURE_LEVEL featureLevels[] = {
			D3D_FEATURE_LEVEL_12_2, D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0
		};
		const char* featureLevelStrings[] = { "12.2", "12.1", "12.0" };
		// 高い順に生成できるか試していく
		for (size_t i = 0; i < _countof(featureLevels); ++i) {
			// 採用したアダプタでデバイスを生成
			hr_ = D3D12CreateDevice(useAdapter_.Get(), featureLevels[i], IID_PPV_ARGS(&device_));
			// 指定した機能レベルでデバイスが生成できたか確認
			if (SUCCEEDED(hr_)) {
				// 生成できたのでログ出力を使ってループを抜ける
				Convert::Log(std::format("FeatureLevel : {}\n", featureLevelStrings[i]));
				break;
			}
		}
		// デバイスの生成がうまくいかなかったので起動できない
		assert(device_ != nullptr);
		Convert::Log("Complete create D3D12Device!!!\n");

#ifdef _DEBUG

		Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
			debugController->EnableDebugLayer();
		}

		ID3D12InfoQueue* infoQueue = nullptr;
		if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
			// やばいエラー時に止まる
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
			// エラー時に止まる
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
			// 警告時に止まる
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

			// 抑制するメッセージのID
			D3D12_MESSAGE_ID denyIds[] = {
				// Window11でのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用バグによるエラーメッセージ
				// https://stackflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
				D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
			};
			// 抑制するレベル
			D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
			D3D12_INFO_QUEUE_FILTER filter{};
			filter.DenyList.NumIDs = _countof(denyIds);
			filter.DenyList.pIDList = denyIds;
			filter.DenyList.NumSeverities = _countof(severities);
			filter.DenyList.pSeverityList = severities;
			// 指定したメッセージの表示を抑制する
			infoQueue->PushStorageFilter(&filter);

			// 解放
			infoQueue->Release();
		}
#endif

		//コマンドキューを生成する
		D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
		hr_ = device_->CreateCommandQueue(&commandQueueDesc,
			IID_PPV_ARGS(&commandQueue_));

		// コマンドキューの生成がうまくいかなかったので起動できない
		assert(SUCCEEDED(hr_));

		// コマンドアロケータを生成する
		hr_ = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
		// コマンドアロケータの生成がうまくいかなかったので起動できない
		assert(SUCCEEDED(hr_));

		// コマンドリストを生成する
		hr_ = device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr,
			IID_PPV_ARGS(&commandList_));
		// コマンドリストの生成がうまくいかなかったので起動できない
		assert(SUCCEEDED(hr_));

		// スワップチェーンを生成する
		swapChainDesc.Width = WinApp::GetInsTance()->GetKClientWidth();// 画面の幅
		swapChainDesc.Height = WinApp::GetInsTance()->GetKClientHeight();// 画面の高さ
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;// 色の形式
		swapChainDesc.SampleDesc.Count = 1;// マルチサンプル
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;// 描画のターゲットとして利用
		swapChainDesc.BufferCount = 2;/// ダブルバッファ
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;// モニタに写したら中身を破棄

		// コマンドキュー、ウインドウハンドル、設定を渡して生成する
		hr_ = dxgiFactory_->CreateSwapChainForHwnd(commandQueue_.Get(), WinApp::GetInsTance()->GetHwnd(), &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf()));
		assert(SUCCEEDED(hr_));

		// RTV用のヒープでディスクリプタの数は2。RTVはShader内で触るものではないので、ShaderVisibleはfalse
		rtvDescriptorHeap_ = CreateDescriptorHeap(device_.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 3, false);

		// SRV用のヒープでディスクリプタの数は128。SRVはShader内で触るものなので、ShaderVisibleはtrue
		srvDescriptorHeap_ = CreateDescriptorHeap(device_.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);
		srvDescriptorHeap2_ = CreateDescriptorHeap(device_.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);

		// SwapChainからResourceを引っ張ってくる
		hr_ = swapChain_->GetBuffer(0, IID_PPV_ARGS(&swapChainResources[0]));
		// うまく取得できなければ起動できない
		assert(SUCCEEDED(hr_));
		hr_ = swapChain_->GetBuffer(1, IID_PPV_ARGS(&swapChainResources[1]));
		assert(SUCCEEDED(hr_));

		// RTVの設定
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

		// ディスクリプタの先頭を取得する
		D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();

		// RTVを2つ作るのでディスクリプタを2つ用意
		// 
		// まず1つ目を作る。１つ目は最初のところに作る。作る場所をこちらで指定してあげる必要がある
		rtvHandles[0] = rtvStartHandle;
		device_->CreateRenderTargetView(swapChainResources[0].Get(), &rtvDesc, rtvHandles[0]);
		// 2つ目のディスクリプタハンドルを得る(自力で)
		rtvHandles[1].ptr = rtvHandles[0].ptr + device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		// 2つ目を作る
		device_->CreateRenderTargetView(swapChainResources[1].Get(), &rtvDesc, rtvHandles[1]);

		// RTVの作成
		renderTextureResource = CreateRenderTextureResource(device_.Get(), WinApp::GetInsTance()->GetKClientWidth(), WinApp::GetInsTance()->GetKClientHeight(), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, kRenderTargetClearValue);
		rtvHandle.ptr = rtvHandles[1].ptr + device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		device_->CreateRenderTargetView(renderTextureResource.Get(), &rtvDesc, rtvHandle);

		srvHandle.ptr += srvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart().ptr + device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		srvGpuHandle.ptr += srvDescriptorHeap_->GetGPUDescriptorHandleForHeapStart().ptr + device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		// SRVの設定。FormatはResourceと同じにしておく
		D3D12_SHADER_RESOURCE_VIEW_DESC renderTextureSrvDesc{};
		renderTextureSrvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		renderTextureSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		renderTextureSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		renderTextureSrvDesc.Texture2D.MipLevels = 1;

		// SRVの作成
		device_->CreateShaderResourceView(renderTextureResource.Get(), &renderTextureSrvDesc, srvHandle);

		// DepthStencilTextureをウィンドウのサイズで作成
		depthStencilResource = CreateDepthStencilTextureResource(device_.Get(), WinApp::GetInsTance()->GetKClientWidth(), WinApp::GetInsTance()->GetKClientHeight());

		// DSV用のヒープでディスクリプタの数は1。DSVはShader内で触るものではないので、shaderVisibleはfalse
		dsvDescriptorHeap_ = CreateDescriptorHeap(device_.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);

		// DSVの設定
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
		dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		// DSVHeapの先頭にDSVを作る
		device_->CreateDepthStencilView(depthStencilResource.Get(), &dsvDesc, dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart());

		DirectXCommon::Fence();
		DirectXCommon::Viewport();
		DirectXCommon::Scissor();
	}

	void DirectXCommon::Fence() {
		// 初期値0でFenceを作る

		hr_ = device_->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
		assert(SUCCEEDED(hr_));

		// Fenceのsignalを待つためにイベントを作成する
		[[maybe_unused]] HANDLE fenceEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);
		assert(fenceEvent_ != nullptr);
	}

	void DirectXCommon::RenderTexture() {

		//これから書き込むバックバッファのインデックスを取得 
		backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

		// 今回のバリアはTransition
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		// Noneにしておく
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		// バリアを張る対象のリソース。現在のバックバッファに対して行う
		barrier.Transition.pResource = swapChainResources[backBufferIndex].Get();
		// 遷移前(現在)のResourceState
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		// 遷移後のResourceState
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		// TransitionBarrierを練る
		commandList_->ResourceBarrier(1, &barrier);

		// 描画先のRTVとDSVをを設定する
		dsvHandle = dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
		commandList_->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

		// 指定した色で画面全体をクリアする
		float clearColor[] = { kRenderTargetClearValue.x,kRenderTargetClearValue.y,kRenderTargetClearValue.z,kRenderTargetClearValue.w };
		commandList_->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

		// 指定した深度で画面全体をクリアする
		commandList_->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		// 描画用のDescriptorHeapの設定
		ID3D12DescriptorHeap* descriptorHeaps2[] = { srvDescriptorHeap2_.Get() };
		commandList_->SetDescriptorHeaps(1, descriptorHeaps2);

		commandList_->RSSetViewports(1, &viewport);
		commandList_->RSSetScissorRects(1, &scissorRect);
	}

	void DirectXCommon::SwapChain()
	{
		// 今回のバリアはTransition
		barrier2.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		// Noneにしておく
		barrier2.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		// バリアを張る対象のリソース。現在のバックバッファに対して行う
		barrier2.Transition.pResource = renderTextureResource.Get();
		// 遷移前(現在)のResourceState
		barrier2.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		// 遷移後のResourceState
		barrier2.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		// TransitionBarrierを練る
		commandList_->ResourceBarrier(1, &barrier2);

		// 描画先のRTVとDSVをを設定する
		dsvHandle = dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
		commandList_->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, &dsvHandle);

		// 指定した色で画面全体をクリアする
		float clearColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		commandList_->ClearRenderTargetView(rtvHandles[backBufferIndex], clearColor, 0, nullptr);

		// 描画用のDescriptorHeapの設定
		ID3D12DescriptorHeap* descriptorHeaps[] = { srvDescriptorHeap_.Get() };
		commandList_->SetDescriptorHeaps(1, descriptorHeaps);

		commandList_->RSSetViewports(1, &viewport);
		commandList_->RSSetScissorRects(1, &scissorRect);
	}

	void DirectXCommon::RemoveBarrier()
	{
		// 遷移前(現在)のResourceState
		barrier2.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		// 遷移後のResourceState
		barrier2.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		// TransitionBarrierを練る
		commandList_->ResourceBarrier(1, &barrier2);
	}

	void DirectXCommon::Close() {
		// 画面に描く処理はすべて終わり、画面に映すので、状態を遷移
		// 今回はRenderTargetからPresentにする
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		// TransitionBarrierを練る
		commandList_->ResourceBarrier(1, &barrier);

		// コマンドリストの内容を確定させる。すべてのコマンドを積んでからcloseすること
		hr_ = commandList_->Close();
		assert(SUCCEEDED(hr_));
		// GPUにコマンドの実行を行わせる
		ID3D12CommandList* commandLists[] = { commandList_.Get() };
		commandQueue_->ExecuteCommandLists(1, commandLists);
		// GPUとOSに画像の交換を行うよう通知する
		swapChain_->Present(1, 0);

		// Fenceの値を更新
		fenceValue++;
		// GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにsignalを送る
		commandQueue_->Signal(fence.Get(), fenceValue);
		// Fenceの値が指定したSignal値にたどりついているか確認する
				// GetCompletedvalueの初期値はFence作成時に渡した初期値
		if (fence->GetCompletedValue() < fenceValue) {
			// 指定したsignalにたどり着いていないので、たどり着くまでイベントを設定する
			fence->SetEventOnCompletion(fenceValue, fenceEvent);
			// イベントをまつ
			WaitForSingleObject(fenceEvent, INFINITE);
		}

		// FPS固定更新
		UpdateFixFPS();

		// 次のフレーム用のコマンドリストを準備
		hr_ = commandAllocator_->Reset();
		assert(SUCCEEDED(hr_));
		hr_ = commandList_->Reset(commandAllocator_.Get(), nullptr);
		assert(SUCCEEDED(hr_));
	}


	void DirectXCommon::Viewport() {
		// ビューポート
		// クライアント領域のサイズと一緒にして画面全体に表示
		viewport.Width = (float)WinApp::GetInsTance()->GetKClientWidth();
		viewport.Height = (float)WinApp::GetInsTance()->GetKClientHeight();
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
	}

	void DirectXCommon::Scissor() {
		// シザー矩形
		// 基本的にビューポートと同じ矩形が構成されるようにする
		scissorRect.left = 0;
		scissorRect.right = WinApp::GetInsTance()->GetKClientWidth();
		scissorRect.top = 0;
		scissorRect.bottom = WinApp::GetInsTance()->GetKClientHeight();
	}


	void DirectXCommon::Release() {

		CloseHandle(fenceEvent);

#ifdef _DEBUG
		WinApp::GetInsTance()->GetDebugController()->Release();
		//Microsoft::WRL::ComPtr<ID3D12DebugDevice> debugDevice;
		//if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&debugDevice)))) {
		//	// 未解放オブジェクトを報告
		//	debugDevice->ReportLiveObjects(D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL);
		//}
#endif
		CloseWindow(WinApp::GetInsTance()->GetHwnd());

		struct D3DResourceLeakCheker {
			~D3DResourceLeakCheker() {
				// リソースリークチェック
				Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
				if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
					debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
					debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
					debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
					debug->Release();
				}
			}
		};
	}

	// DescriptorHeap関数
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DirectXCommon::CreateDescriptorHeap(Microsoft::WRL::ComPtr<ID3D12Device> device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible)
	{
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap = nullptr;
		// ディスクリプタヒープの生成
		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
		descriptorHeapDesc.Type = heapType;
		descriptorHeapDesc.NumDescriptors = numDescriptors;
		descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		[[maybe_unused]] HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
		// ディスクリプタヒープが作れなかったので起動できない
		assert(SUCCEEDED(hr));
		return descriptorHeap;
	}

	Microsoft::WRL::ComPtr<ID3D12Resource> DirectXCommon::CreateDepthStencilTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, int32_t width, int32_t height)
	{
		// 生成するResourceの設定
		D3D12_RESOURCE_DESC resourceDesc{};
		resourceDesc.Width = width; // Textureの幅
		resourceDesc.Height = height; // Textureの高さ
		resourceDesc.MipLevels = 1; // mipmapの数
		resourceDesc.DepthOrArraySize = 1; // 奥行 or 配列Textureの配列数
		resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // DepthStencilとして利用可能なフォーマット
		resourceDesc.SampleDesc.Count = 1; // サンプリングカウント
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; // 2次元
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; // DepthStencilとして使う通知

		// 利用するHeapの設定
		D3D12_HEAP_PROPERTIES heapProperties{};
		heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; // VRAM上に作る

		// 深度値のクリア設定
		D3D12_CLEAR_VALUE depthClearValue{};
		depthClearValue.DepthStencil.Depth = 1.0f;
		depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

		// Resourceの生成
		Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
		[[maybe_unused]] HRESULT hr = device->CreateCommittedResource(
			&heapProperties, // Heapの設定
			D3D12_HEAP_FLAG_NONE, // Heapの特殊設定。特になし。
			&resourceDesc, // Resourceの設定
			D3D12_RESOURCE_STATE_DEPTH_WRITE, // 深度値を書き込む状態にしておく
			&depthClearValue, // Clear最適値
			IID_PPV_ARGS(&resource)); // 作成するResourceポインタへのポインタ
		assert(SUCCEEDED(hr));

		return resource;
	}

	Microsoft::WRL::ComPtr<ID3D12Resource> DirectXCommon::CreateRenderTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, int32_t width, int32_t height, DXGI_FORMAT format, const Vector4& clearColor)
	{
		D3D12_RESOURCE_DESC resourceDesc{};
		resourceDesc.Width = width; // Textureの幅
		resourceDesc.Height = height; // Textureの高さ
		resourceDesc.MipLevels = 1; // mipmapの数
		resourceDesc.DepthOrArraySize = 1; // 奥行 or 配列Textureの配列数
		resourceDesc.Format = format; // DepthStencilとして利用可能なフォーマット
		resourceDesc.SampleDesc.Count = 1; // サンプリングカウント
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; // 2次元
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET; // RenderTargetとして利用可能にする

		// 利用するHeapの設定
		D3D12_HEAP_PROPERTIES heapProperties{};
		heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; // VRAM上に作る

		// clearValue
		D3D12_CLEAR_VALUE clearValue;
		clearValue.Format = format;
		clearValue.Color[0] = clearColor.x;
		clearValue.Color[1] = clearColor.y;
		clearValue.Color[2] = clearColor.z;
		clearValue.Color[3] = clearColor.w;

		// Resourceの生成
		Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
		[[maybe_unused]] HRESULT hr = device->CreateCommittedResource(
			&heapProperties, // Heapの設定
			D3D12_HEAP_FLAG_NONE, // Heapの特殊設定。特になし。
			&resourceDesc, // Resourceの設定
			D3D12_RESOURCE_STATE_RENDER_TARGET, // これから描画することを前提としたTextureなのでRenderTaregtとして使うことから始める
			&clearValue, // Clear最適値。ClearRenderTargetをこの色でClearするようにする。最適化されているので高速である。
			IID_PPV_ARGS(&resource)); // 作成するResourceポインタへのポインタ
		assert(SUCCEEDED(hr));

		return resource;
	}

	void DirectXCommon::InitializeFixFPS() {
		// 現在時間を記録する
		reference_ = std::chrono::steady_clock::now();
	}

	void DirectXCommon::UpdateFixFPS() {
		// 1/60秒ピッタリの時間
		const std::chrono::microseconds kMinTime(uint64_t(1000000.0f / 60.0f));
		// 1/60秒よりわずかに短い時間
		const std::chrono::microseconds kMinCheckTime(uint64_t(1000000.0f / 65.0f));

		// 現在時間を取得
		std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
		// 前回時間からの経過時間を取得する
		std::chrono::microseconds elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);

		// 1/60秒(よりわずかに短い時間)経っていない場合
		if (elapsed < kMinCheckTime) {
			// 1/60秒経過するまで微小なスリープを繰り返す
			while (std::chrono::steady_clock::now() - reference_ < kMinTime) {
				// 1マイクロ秒スリープ
				std::this_thread::sleep_for(std::chrono::microseconds(1));
			}
		}

		// 現在時間を記録
		reference_ = std::chrono::steady_clock::now();
	}

}