#include "Particle.h"
#include <numbers>

/// <summary>
/// Particle.cpp
/// パーティクル生成のソースファイル
/// </summary>

namespace Engine
{
	Particles::~Particles()
	{
	}

	void Particles::Initialize(const std::string& filename, Vector3 pos) {

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
		instancingResource = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(ParticleForGpu) * kMaxInstance);
		instancingData_ = nullptr;
		instancingResource->Map(0, nullptr, reinterpret_cast<void**>(&instancingData_));

		// SRVの作成
		SrvIndex_ = srvManager_->Allocate();
		srvManager_->CreateSRVforStructuredBuffer(SrvIndex_, instancingResource.Get(), kMaxInstance, sizeof(ParticleForGpu));
		instancingSrvHandleCPU_ = srvManager_->GetCPUDescriptorHandle(SrvIndex_);
		instancingSrvHandleGPU_ = srvManager_->GetGPUDescriptorHandle(SrvIndex_);

		Particles::CreateVertexResource();
		Particles::CreateMaterialResource();
		Particles::CreateWVPResource();

		std::random_device seedGenerator;
		std::mt19937 randomEngine(seedGenerator());

		int index_ = 0;

		// 位置と速度を[-1,1]でランダムに初期化
		for (std::list<Particle>::iterator particleItelater = RoopParticles.begin(); particleItelater != RoopParticles.end(); ++particleItelater, ++index_) {
			instancingData_[index_].WVP = MakeIndentity4x4();
			instancingData_[index_].World = MakeIndentity4x4();
			instancingData_[index_].color = { 1.0f, 1.0f, 1.0f, 1.0f };

			RoopParticles.push_back(MakeNewParticle(randomEngine));
			instancingData_[index_].color = particleItelater->color;
			particleItelater->transform.translate = pos;
		}

		for (uint32_t index = 0; index < kMaxInstance; ++index) {
			instancingData_[index].WVP = MakeIndentity4x4();
			instancingData_[index].World = MakeIndentity4x4();
			instancingData_[index].color = { 1.0f, 1.0f, 1.0f, 1.0f };
			particles[index] = MakeNewParticle(randomEngine);
			instancingData_[index].color = particles[index].color;
			particles[index].transform.translate = pos;
		}

		backToFrontMatrix = MakeRotateYMatrix(std::numbers::pi_v<float>);
	}

	void Particles::Update() {
	}

	void Particles::Draw(Camera* camera_, uint32_t index) {

		uint32_t numInstance = 0;
		for (uint32_t index_ = 0; index_ < kMaxInstance; ++index_) {
			if (particles[index_].lifeTime <= particles[index_].currentTime) {
				continue;
			}

			Matrix4x4 worldMatrix = MakeAffineMatrix(particles[index_].transform.scale, GetBillboard(camera_), particles[index_].transform.translate);
			Matrix4x4 worldViewMatrix = Multiply(worldMatrix, Multiply(worldMatrix, Multiply(camera_->viewMatrix, camera_->projectionMatrix)));

			particles[index_].transform.translate.x += particles[index_].velocity.x * kDeltaTime;
			particles[index_].transform.translate.y += particles[index_].velocity.y * kDeltaTime;
			particles[index_].transform.translate.z += particles[index_].velocity.z * kDeltaTime;
			particles[index_].currentTime += kDeltaTime;

			instancingData_[index_].World = worldMatrix;
			instancingData_[index_].WVP = Multiply(worldMatrix, Multiply(camera_->viewMatrix, camera_->projectionMatrix));
			instancingData_[index_].color = particles[index_].color;

			float alpha = 1.0f - (particles[index_].currentTime / particles[index_].lifeTime);
			instancingData_[numInstance].color.w = alpha;

			++numInstance;
		}

		/*Matrix4x4 uvtransformMatrix = MakeScaleMatrix(uvTransform.scale);
		uvtransformMatrix = Multiply(uvtransformMatrix, MakeRotateZMatrix(uvTransform.rotate.z));
		uvtransformMatrix = Multiply(uvtransformMatrix, MakeTranslateMatrix(uvTransform.translate));
		materialData->uvTransform = uvtransformMatrix;*/

		/*ID3D12DescriptorHeap* descriptorHeaps[] = { DirectXCommon::GetInsTance()->GetSrvDescriptorHeap2().Get()};
		DirectXCommon::GetInsTance()->GetCommandList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);*/

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
	}

	void Particles::RoopDraw(Camera* camera_, uint32_t index) {

		int index_ = 0;
		uint32_t numInstance = 0;

		for (std::list<Particle>::iterator particleItelater = RoopParticles.begin(); particleItelater != RoopParticles.end(); ++index_) {

			if ((*particleItelater).lifeTime <= (*particleItelater).currentTime) {
				continue;
			}

			Matrix4x4 worldMatrix = MakeAffineMatrix(particleItelater->transform.scale, GetBillboard(camera_), particleItelater->transform.translate);
			Matrix4x4 worldViewMatrix = Multiply(worldMatrix, Multiply(worldMatrix, Multiply(camera_->viewMatrix, camera_->projectionMatrix)));

			particleItelater->transform.translate.x += particleItelater->velocity.x * kDeltaTime;
			particleItelater->transform.translate.y += particleItelater->velocity.y * kDeltaTime;
			particleItelater->transform.translate.z += particleItelater->velocity.z * kDeltaTime;
			particleItelater->currentTime += kDeltaTime;

			instancingData_[index_].World = worldMatrix;
			instancingData_[index_].WVP = Multiply(worldMatrix, Multiply(camera_->viewMatrix, camera_->projectionMatrix));
			instancingData_[index_].color = particleItelater->color;

			float alpha = 1.0f - (particleItelater->currentTime / particleItelater->lifeTime);
			instancingData_[numInstance].color.w = alpha;

			++numInstance;
			++particleItelater;
		}

		/*Matrix4x4 uvtransformMatrix = MakeScaleMatrix(uvTransform.scale);
		uvtransformMatrix = Multiply(uvtransformMatrix, MakeRotateZMatrix(uvTransform.rotate.z));
		uvtransformMatrix = Multiply(uvtransformMatrix, MakeTranslateMatrix(uvTransform.translate));
		materialData->uvTransform = uvtransformMatrix;*/

		/*ID3D12DescriptorHeap* descriptorHeaps[] = { DirectXCommon::GetInsTance()->GetSrvDescriptorHeap2().Get()};
		DirectXCommon::GetInsTance()->GetCommandList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);*/

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
		for (uint32_t index_ = 0; index_ < kMaxInstance; ++index_) {
			particles[index_].transform.translate = pos_;
		}
	}

	void Particles::SetRoopPos(Vector3 pos_)
	{
		for (std::list<Particle>::iterator particleItelater = RoopParticles.begin(); particleItelater != RoopParticles.end(); ++particleItelater) {
			particleItelater->transform.translate = pos_;
		}
	}

	Particle Particles::MakeNewParticle(std::mt19937& randomEngine)
	{
		std::uniform_real_distribution<float> distribution(-8.0f, 8.0f);
		std::uniform_real_distribution<float> distColor(1.0f, 1.0f);
		std::uniform_real_distribution<float> distTime(1.0f, 3.0f);
		Particle particle;
		// 位置と速度を[-1,1]でランダムに初期化
		particle.transform.scale = { 5.0f,5.0f,5.0f };
		particle.transform.rotate = { 0,0,0 };
		particle.transform.translate = { distribution(randomEngine), distribution(randomEngine), distribution(randomEngine) };
		particle.velocity = { distribution(randomEngine), distribution(randomEngine), distribution(randomEngine) };
		particle.color = { distColor(randomEngine), distColor(randomEngine), distColor(randomEngine), 1.0f };
		particle.lifeTime = distTime(randomEngine);
		particle.currentTime = 0;
		return particle;
	}
}

