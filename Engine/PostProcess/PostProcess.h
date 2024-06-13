#pragma once
#include "DirectXCommon.h"

class PostProcess {
public:
	void Initialize();
	void Draw();
private:
	void CreatePSO();
private:
	DirectXCommon* dir_ = DirectXCommon::GetInsTance();

	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_ = nullptr;
};