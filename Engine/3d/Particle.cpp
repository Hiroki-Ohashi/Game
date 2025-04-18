#include "Particle.h"
#include <numbers>
#include <iostream>

/// <summary>
/// Particle.cpp
/// パーティクル生成のソースファイル
/// </summary>

namespace Engine
{

	Particles::~Particles()
	{
	}

	void Particles::Initialize(const std::string& filename, Vector3 pos, Emitter emitter_) {

		// モデル読み込み
		const std::wstring filePathW = Convert::ConvertString(filename);;
		if (filePathW.ends_with(L".obj")) {
			modelData = texture_->LoadObjFile("resources", filename);
		}
		else {
			modelData = texture_->LoadModelFile("resources", filename);
		}
		DirectX::ScratchImage mipImages2 = texture_->LoadTexture(modelData.material.textureFilePath);

		// 頂点の座標
		modelData.vertices.push_back({ .position = {-1.0f,1.0f,0.0f,1.0f}, .texcoord = {0.0f,0.0f},.normal = {0.0f,0.0f,1.0f} }); // 左上
		modelData.vertices.push_back({ .position = {1.0f,1.0f,0.0f,1.0f}, .texcoord = {1.0f,0.0f},.normal = {0.0f,0.0f,1.0f} }); // 右上
		modelData.vertices.push_back({ .position = {-1.0f,-1.0f,0.0f,1.0f}, .texcoord = {0.0f,1.0f},.normal = {0.0f,0.0f,1.0f} }); // 左下

		modelData.vertices.push_back({ .position = {-1.0f,-1.0f,0.0f,1.0f}, .texcoord = {0.0f,1.0f},.normal = {0.0f,0.0f,1.0f} }); // 左下
		modelData.vertices.push_back({ .position = {1.0f,1.0f,0.0f,1.0f}, .texcoord = {1.0f,0.0f},.normal = {0.0f,0.0f,1.0f} }); // 右上
		modelData.vertices.push_back({ .position = {1.0f,-1.0f,0.0f,1.0f}, .texcoord = {1.0f,1.0f},.normal = {0.0f,0.0f,1.0f} }); // 右下

		// Resource作成
		instancingResource = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(ParticleForGpu) * kNumMaxInstance);
		instancingData_ = nullptr;
		instancingResource->Map(0, nullptr, reinterpret_cast<void**>(&instancingData_));

		// SRVの作成
		SrvIndex_ = srvManager_->Allocate();
		srvManager_->CreateSRVforStructuredBuffer(SrvIndex_, instancingResource.Get(), kNumMaxInstance, sizeof(ParticleForGpu));
		instancingSrvHandleCPU_ = srvManager_->GetCPUDescriptorHandle(SrvIndex_);
		instancingSrvHandleGPU_ = srvManager_->GetGPUDescriptorHandle(SrvIndex_);

		Particles::CreateVertexResource();
		Particles::CreateMaterialResource();
		Particles::CreateWVPResource();

		int index_ = 0;

		// 位置と速度を[-1,1]でランダムに初期化
		for (std::list<Particle>::iterator particleItelater = particles.begin(); particleItelater != particles.end(); ++particleItelater, ++index_) {
			instancingData_[index_].WVP = MakeIndentity4x4();
			instancingData_[index_].World = MakeIndentity4x4();
			instancingData_[index_].color = { 1.0f, 1.0f, 1.0f, 1.0f };
			instancingData_[index_].color = particleItelater->color;
			particleItelater->transform.translate = pos;
		}

		backToFrontMatrix = MakeRotateYMatrix(std::numbers::pi_v<float>);

		this->emitter.transform.translate = { 0.0f, 0.0f, 0.0f };
		this->emitter.transform.rotate = { 0.0f, 0.0f, 0.0f };
		this->emitter.transform.scale = { 1.0f, 1.0f, 1.0f };
		this->emitter = emitter_;
	}

	void Particles::Update() {

		std::random_device seedGenerator;
		std::mt19937 randomEngine(seedGenerator());

		emitter.frequencyTime += kDeltaTime;
		if (emitter.frequency <= emitter.frequencyTime) {
			particles.splice(particles.end(), Emit(emitter, randomEngine));
			emitter.frequencyTime = 0.0f;
		}
	}

	void Particles::EmitOnce(const Emitter& emitter_) {
		std::random_device seedGenerator;
		std::mt19937 randomEngine(seedGenerator());
		particles.splice(particles.end(), EmitCircle(emitter_, randomEngine));
	}

	void Particles::UpdateEmitterPosition(const Vector3& pos) {
		emitter.transform.translate = pos;
	}

	void Particles::Draw(Camera* camera_, uint32_t index) {

		uint32_t numInstance = 0;
		for (std::list<Particle>::iterator particleItelater = particles.begin(); particleItelater != particles.end();) {

			if (particleItelater->lifeTime <= particleItelater->currentTime) {
				particleItelater = particles.erase(particleItelater);
				continue;
			}

			Matrix4x4 worldMatrix = MakeAffineMatrix(particleItelater->transform.scale, GetBillboard(camera_), particleItelater->transform.translate);
			Matrix4x4 viewProjection = Multiply(camera_->viewMatrix, camera_->projectionMatrix);

			// 移動処理
			particleItelater->transform.translate.x += particleItelater->velocity.x * kDeltaTime;
			particleItelater->transform.translate.y += particleItelater->velocity.y * kDeltaTime;
			particleItelater->transform.translate.z += particleItelater->velocity.z * kDeltaTime;
			particleItelater->currentTime += kDeltaTime;

			if (numInstance < kNumMaxInstance) {
				// インスタンシング用データの設定
				instancingData_[numInstance].World = worldMatrix;
				instancingData_[numInstance].WVP = Multiply(worldMatrix, viewProjection);
				instancingData_[numInstance].color = particleItelater->color;

				float alpha = 1.0f - (particleItelater->currentTime / particleItelater->lifeTime);
				instancingData_[numInstance].color.w = alpha;

				++numInstance;
			}

			++particleItelater;
		}

		// DirectXCommon::GetInsTance()を設定。PSOに設定しているけど別途設定が必要
		DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootSignature(pipeLineManager_->GetRootSignatureParticle().Get());
		DirectXCommon::GetInstance()->GetCommandList()->SetPipelineState(pipeLineManager_->GetGraphicsPipelineStateParticle().Get());

		// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
		DirectXCommon::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// コマンドを積む
		DirectXCommon::GetInstance()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView); // VBVを設定
		// マテリアルCBufferの場所を設定
		DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
		// TransformationMatrixCBufferの場所を設定
		DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootDescriptorTable(1, instancingSrvHandleGPU_);
		// SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である。
		DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootDescriptorTable(2, srvManager_->GetGPUDescriptorHandle(index));

		DirectXCommon::GetInstance()->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), numInstance, 0, 0);

		if (ImGui::TreeNode("Particle")) {
			ImGui::Text("draw");
			ImGui::TreePop();
		}
	}

	void Particles::Release() {
	}

	Vector3 Particles::GetBillboard(Camera* camera_)
	{
		Vector3 billboard;

		Matrix4x4 billboardMatrix = Multiply(backToFrontMatrix, camera_->cameraMatrix);
		billboard.x = billboardMatrix.m[3][0] = 0.0f;
		billboard.y = billboardMatrix.m[3][1] = 0.0f;
		billboard.z = billboardMatrix.m[3][2] = 0.0f;

		return billboard;
	}

	void Particles::CreateVertexResource() {
		// 頂点用のリソースを作る。
		vertexResource = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(VertexData) * modelData.vertices.size()).Get();

		// リソースの先頭のアドレスから使う
		vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
		// 使用するリソースのサイズは頂点3つ分のサイズ
		vertexBufferView.SizeInBytes = sizeof(VertexData) * UINT(modelData.vertices.size());
		// 1頂点あたりのサイズ
		vertexBufferView.StrideInBytes = sizeof(VertexData);

		// 頂点リソースにデータを書き込む
		vertexData = nullptr;

		// 書き込むためのアドレスを取得
		vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
		// 頂点データをリソースにコピー
		std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());
	}

	void Particles::CreateMaterialResource() {
		// マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
		materialResource = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(Material));
		// マテリアルにデータを書き込む
		materialData = nullptr;
		// 書き込むためのアドレスを取得
		materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
		// 白を設定
		materialData->color = { 1.0f, 1.0f, 1.0f, 1.0f };

		materialData->uvTransform = MakeIndentity4x4();

		materialData->enableLighting = false;
	}

	void Particles::CreateWVPResource() {
		// WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
		wvpResource = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(TransformationMatrix));

		// 書き込むためのアドレスを取得
		wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));

		// 単位行列を書き込んでおく
		wvpData->WVP = MakeIndentity4x4();
	}

	Microsoft::WRL::ComPtr<ID3D12Resource> Particles::CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInbytes)
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;

		D3D12_HEAP_PROPERTIES uploadHeapProperties{};
		// 頂点リソース用のヒープの設定
		uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;// UploadHeapを使う

		// 頂点リソースの設定
		D3D12_RESOURCE_DESC ResourceDesc{};
		// バッファリソース。テクスチャの場合はまた別の設定をする
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Width = sizeInbytes;
		// バッファの場合はこれらは1にする決まり
		ResourceDesc.Height = 1;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.SampleDesc.Count = 1;
		// バッファの場合はこれにする決まり
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		// 実際に頂点リソースを作る
		[[maybe_unused]]
		HRESULT hr = device->CreateCommittedResource(
			&uploadHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&ResourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&Resource));

		assert(SUCCEEDED(hr));

		return Resource;
	}

	void Particles::SetPos(Vector3 pos_)
	{
		for (std::list<Particle>::iterator particleItelater = particles.begin(); particleItelater != particles.end(); ++particleItelater) {
			particleItelater->transform.translate = pos_;
		}
	}

	Particle Particles::MakeNewParticle(std::mt19937& randomEngine, const Vector3& translate, const Vector3& rotation)
	{
		std::uniform_real_distribution<float> distribution(-0.5f, 0.5f); // ちょっとだけ揺らす
		std::uniform_real_distribution<float> distColor(1.0f, 1.0f);
		std::uniform_real_distribution<float> distTime(3.0f, 5.0f); // 少し長めに見せる
		std::uniform_real_distribution<float> distZ(-0.1f, 8.1f);

		Particle particle;
		particle.transform.scale = { 0.7f, 0.7f, 0.7f };
		particle.transform.rotate = { 0,0,0 };

		// 向いている方向（Z前方）をもとに後ろ向きベクトルを出す
		Vector3 forward = {
			std::sin(rotation.y),
			0.0f,
			std::cos(rotation.y)
		};
		Vector3 offset = {
			distribution(randomEngine),
			distribution(randomEngine),
			distZ(randomEngine)
		};

		// プレイヤーの位置に配置（揺らしつき）
		particle.transform.translate = {
			translate.x + offset.x,
			translate.y + offset.y,
			translate.z + offset.z
		};

		// 後方に向かって飛ばす（forwardをマイナス）
		particle.velocity.x = forward.x * -5.0f + offset.x;
		particle.velocity.y = forward.y * -5.0f + offset.y;
		particle.velocity.z = forward.z * -5.0f + offset.z;

		particle.color = { distColor(randomEngine), distColor(randomEngine), distColor(randomEngine), 1.0f };
		particle.lifeTime = 0.02f;
		particle.currentTime = 0;
		return particle;
	}

	Particle Particles::MakeNewCircleParticle(std::mt19937& randomEngine, const Vector3& translate)
	{
		std::uniform_real_distribution<float> distXYZ(-1.0f, 1.0f);
		std::uniform_real_distribution<float> distSpeed(8.0f, 16.0f);
		std::uniform_real_distribution<float> distTime(1.0f, 3.0f);
		std::uniform_real_distribution<float> distColor(1.0f, 1.0f);

		// ランダムな方向ベクトル（正規化する）
		Vector3 direction = {
			distXYZ(randomEngine),
			distXYZ(randomEngine),
			distXYZ(randomEngine)
		};

		// 正規化
		float length = std::sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
		if (length != 0.0f) {
			direction.x /= length;
			direction.y /= length;
			direction.z /= length;
		}

		// スピードを乗算して放射方向に飛ばす
		float speed = distSpeed(randomEngine);
		Vector3 velocity = {
			direction.x * speed,
			direction.y * speed,
			direction.z * speed
		};

		// 生成
		Particle particle;
		particle.transform.translate = translate;
		particle.transform.scale = { 15.0f, 15.0f, 15.0f };
		particle.transform.rotate = { 0.0f, 0.0f, 0.0f };
		particle.velocity = velocity;
		particle.color = { distColor(randomEngine), distColor(randomEngine), distColor(randomEngine), 1.0f };
		particle.lifeTime = distTime(randomEngine);
		particle.currentTime = 0.0f;

		return particle;
	}

	std::list<Particle> Particles::Emit(const Emitter& emitter_, std::mt19937& randomEngine)
	{
		std::list<Particle> emitterParticles;
		for (uint32_t count = 0; count < emitter_.count; ++count) {
			emitterParticles.push_back(MakeNewParticle(randomEngine, emitter_.transform.translate, emitter_.transform.rotate));
		}
		return emitterParticles;
	}

	std::list<Particle> Particles::EmitCircle(const Emitter& emitter_, std::mt19937& randomEngine)
	{
		std::list<Particle> emitterParticles;
		for (uint32_t count = 0; count < emitter_.count; ++count) {
			emitterParticles.push_back(MakeNewCircleParticle(randomEngine, emitter_.transform.translate));
		}
		return emitterParticles;
	}
}

