#pragma once
#include "DirectXCommon.h"


enum Type {
	NONE,
	GRAY,
	VIGNETTE,
	BOX,
	GAUSSIAN,
	HSV,
	NOISE
};

struct NoiseParams {
	float time;
};

class PostProcess {
public:
	void Initialize(Type type);

	void NiseUpdate(float time);

	void Draw();
	void NoiseDraw();

private:
	void CreatePSO();
	void CreateGrayPSO();
	void CreateVignettePSO();
	void CreateBoxPSO();
	void CreateGaussianPSO();
	void CreateHSVPSO();
	void CreateNoisePSO();

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInbytes);
private:
	DirectXCommon* dir_ = DirectXCommon::GetInsTance();

	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_ = nullptr;

	NoiseParams* noiseData_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> noise_;
};