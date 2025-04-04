#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <fstream>
#include <sstream>
#include <Windows.h>
#include <cassert>
#include <cstdint>
#include <wrl.h>

#include "DirectXCommon.h"
namespace Engine {
	class PipeLineManager {
	public:
		// シングルトン
		static PipeLineManager* GetInstance();
		// 初期化
		void Initialize();

		// Model用PSO作成
		void CreateModelPSO();
		// Sprite用PSO作成
		void CreateSpritePSO();
		// Particle用PSO作成
		void CreateParticlePSO();

		// Getter
		Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSignature() {return rootSignature;}
		Microsoft::WRL::ComPtr<ID3D12PipelineState> GetGraphicsPipelineState() { return graphicsPipelineState; }

		Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSignatureSprite() { return rootSignatureSprite; }
		Microsoft::WRL::ComPtr<ID3D12PipelineState> GetGraphicsPipelineStateSprite() { return graphicsPipelineStateSprite; }

		Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSignatureParticle() { return rootSignatureParticle; }
		Microsoft::WRL::ComPtr<ID3D12PipelineState> GetGraphicsPipelineStateParticle() { return graphicsPipelineStateParticle; }
	private:
		// model
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;

		// sprite
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignatureSprite = nullptr;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateSprite = nullptr;

		// particle
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignatureParticle = nullptr;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateParticle = nullptr;
	private:
		PipeLineManager() = default;
		~PipeLineManager() = default;
		PipeLineManager(PipeLineManager&) = delete;
		PipeLineManager& operator = (PipeLineManager&) = delete;
	};
}