#pragma once
#include "DirectXCommon.h"


enum Type {
	GRAY,
	VIGNETTE,
	BOX,
	GAUSSIAN
};

class PostProcess {
public:
	void Initialize(Type type);
	void Draw();

private:
	void CreateGrayPSO();
	void CreateVignettePSO();
	void CreateBoxPSO();
	void CreateGaussianPSO();
private:
	DirectXCommon* dir_ = DirectXCommon::GetInsTance();

	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_ = nullptr;
};