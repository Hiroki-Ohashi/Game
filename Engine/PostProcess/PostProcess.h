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
	float lineStrength;
	float noiseStrength;
	float vignetteLight;
	float vignetteShape;
};

class PostProcess {
public:
	void Initialize(Type type);

	void NoiseUpdate(float time_);
	void VignetteFadeIn(float light_, float shape_);
	void VignetteFadeOut(float light_, float shape_, float lightEnd, float shapeEnd);

	void Draw();
	void NoiseDraw();

public:

	// vignetteのGetterとSetter
	float GetVignetteLight() { return noiseData_->vignetteLight; }
	void SetVignetteLight(float light_) { noiseData_->vignetteLight = light_; }

	float GetVignetteShape() { return noiseData_->vignetteShape; }
	void SetVignetteShape(float shape_) { noiseData_->vignetteShape = shape_; }

	void SetVignette(float light_, float shape_){ 
		noiseData_->vignetteLight = light_;
		noiseData_->vignetteShape = shape_;
	}

	// noiseのGetterとSetter
	float GetLineStrength() { return noiseData_->lineStrength; }
	void SetLineStrength(float lineStrength_) { noiseData_->lineStrength = lineStrength_; }

	float GetNoiseStrength() { return noiseData_->noiseStrength; }
	void SetNoiseStrength(float noiseStrength_) { noiseData_->noiseStrength = noiseStrength_; }

	void SetNoise(float lineStrength_, float noiseStrength_) {
		noiseData_->lineStrength = lineStrength_;
		noiseData_->noiseStrength = noiseStrength_;
	}

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