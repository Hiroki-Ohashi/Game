#pragma once
#include "DirectXCommon.h"
#include "SrvManager.h"

/// <summary>
/// PostProcess.h
/// ポストエフェクトのヘッダーファイル
/// </summary>

// エフェクト
enum Type {
	NONE,
	GRAY,
	VIGNETTE,
	BOX,
	GAUSSIAN,
	HSV,
	NOISE,
	RADIAL
};

// noisePamam
struct NoiseParams {
	float time;
	float lineStrength;
	float noiseStrength;
	float vignetteLight;
	float vignetteShape;
	float blurStrength;
	int sampleCount;
	float fogStart;
	float fogDensity;
};

namespace Engine
{
	// postProcessクラス
	class PostProcess {
	public:
		// 初期化処理
		void Initialize(Type type);
		// 更新処理
		void NoiseUpdate(float time_);
		void VignetteFadeIn(float light_, float shape_);
		void VignetteFadeOut(float light_, float shape_, float lightEnd, float shapeEnd);
		// 描画処理
		void Draw();
		void NoiseDraw();

	public:

		// vignetteのGetterとSetter
		float GetVignetteLight() { return noiseData_->vignetteLight; }
		void SetVignetteLight(float light_) { noiseData_->vignetteLight = light_; }

		float GetVignetteShape() { return noiseData_->vignetteShape; }
		void SetVignetteShape(float shape_) { noiseData_->vignetteShape = shape_; }

		void SetVignette(float light_, float shape_) {
			noiseData_->vignetteLight = light_;
			noiseData_->vignetteShape = shape_;
		}

		// blurのGetterとSetter
		float GetBlurStrength() { return noiseData_->blurStrength; }
		void SetBlurStrength(float blurStrength_) { noiseData_->blurStrength = blurStrength_; }

		int GetSampleCount() { return noiseData_->sampleCount; }
		void SetSampleCount(int sampleCount_) { noiseData_->sampleCount = sampleCount_; }

		void SetBlur(float blurStrength_, int sampleCount_) {
			noiseData_->blurStrength = blurStrength_;
			noiseData_->sampleCount = sampleCount_;
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

		// fogのGetterとSetter
		float GetFogStart() { return noiseData_->fogStart; }
		void SetFogStart(float fogStart_) { noiseData_->fogStart = fogStart_; }

		float GetFogDensity() { return noiseData_->fogDensity; }
		void SetFogDensity(float fogDensity_) { noiseData_->fogDensity = fogDensity_; }

		void SetFog(float fogStart_, float fogDensity_) {
			noiseData_->fogStart = fogStart_;
			noiseData_->fogDensity = fogDensity_;
		}

	private:
		// PSO生成
		void CreatePSO();
		void CreateGrayPSO();
		void CreateVignettePSO();
		void CreateBoxPSO();
		void CreateGaussianPSO();
		void CreateHSVPSO();
		void CreateNoisePSO();
		void CreateRadialPSO();

		// リソース作成
		Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInbytes);

	private:
		// シングルトン呼び出し
		DirectXCommon* dir_ = DirectXCommon::GetInstance();

		// PSO
		Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_ = nullptr;

		// noise
		NoiseParams* noiseData_ = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> noise_;
	};
}