#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <random>

#include "WinApp.h"
#include "Function.h"
#include "MathFunction.h"
#include "DirectXCommon.h"
#include "TextureManager.h"
#include "Camera.h"
#include "WorldTransform.h"
#include "SrvManager.h"
#include "PipeLineManager.h"


/// <summary>
/// Particle.h
/// パーティクル生成のヘッダーファイル
/// </summary>
 
namespace Engine
{
	// Particleクラス
	class Particles {
	public:
		// デストラクタ
		~Particles();

		// 初期化処理
		void Initialize(const std::string& filename, Vector3 pos, Emitter emitter_);
		// 更新処理
		void Update();
		void UpdateEmitterPosition(const Vector3& pos);
		void EmitOnce(const Emitter& emitter);
		// 描画処理
		void Draw(Camera* camera, uint32_t index);
		// 解放処理
		void Release();

		// Getter
		Vector3 GetBillboard(Camera* camera_);
		Vector3 GetForwardVector(const Vector3& rotation) {
			float yaw = rotation.y;
			return {
				std::sin(yaw),
				0.0f,
				std::cos(yaw)
			};
		}

		// Setter
		void SetPos(Vector3 pos_);
		void SetEmitter(Emitter emitter_) { emitter.transform = emitter_.transform; }

		// ランダム生成
		Particle MakeNewParticle(std::mt19937& randomEngine, const Vector3& translate, const Vector3& rotation);
		Particle MakeNewCircleParticle(std::mt19937& randomEngine, const Vector3& translate);
		// emitter
		std::list<Particle> Emit(const Emitter& emitter_, std::mt19937& randomEngine);
		std::list<Particle> EmitCircle(const Emitter& emitter_, std::mt19937& randomEngine);

	private:
		// vertex生成
		void CreateVertexResource();
		// material作成
		void CreateMaterialResource();
		// wvp作成
		void CreateWVPResource();

		// Resource生成
		Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInbytes);
	private:
		// シングルトン呼び出し
		WinApp* winapp_ = WinApp::GetInstance();
		TextureManager* texture_ = TextureManager::GetInstance();
		SrvManager* srvManager_ = SrvManager::GetInstance();
		PipeLineManager* pipeLineManager_ = PipeLineManager::GetInstance();

		// Transform
		WorldTransform worldTransform_;
		EulerTransform uvTransform;

		// model
		ModelData modelData;

		// BufferView
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

		// Resource
		Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
		Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
		Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource;
		Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource;

		// Data
		VertexData* vertexData;
		Material* materialData;
		TransformationMatrix* wvpData;
		ParticleForGpu* instancingData_;

		// instanceの最大数
		const static uint32_t kNumMaxInstance = 10000;

		std::list<Particle> particles;

		// DiscripterHndle
		D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU_;
		D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU_;

		// 描画条件
		bool isModel;

		// BillBorad
		Matrix4x4 backToFrontMatrix;

		// camera
		CameraForGpu camera;

		// deltaTime
		const float kDeltaTime = 1.0f / 60.0f;

		// srv
		uint32_t SrvIndex_;

		// emitter
		Emitter emitter{};
	};
}
