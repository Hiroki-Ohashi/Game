#include "Particle.h"

/// <summary>
/// Particle.cpp
/// パーティクル生成のソースファイル
/// </summary>

namespace Engine
{
	Particles::~Particles()
	{
	}

	void Particles::Initialize(const std::string& filename, Vector3 pos, uint32_t index) {

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
		instancingResource = CreateBufferResource(DirectXCommon::GetInsTance()->GetDevice(), sizeof(ParticleForGpu) * kMaxInstance);
		instancingData_ = nullptr;
		instancingResource->Map(0, nullptr, reinterpret_cast<void**>(&instancingData_));

		// SRVの作成
		uint32_t descriptorSizeSRV = DirectXCommon::GetInsTance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc{};
		instancingSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
		instancingSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		instancingSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		instancingSrvDesc.Buffer.FirstElement = 0;
		instancingSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		instancingSrvDesc.Buffer.NumElements = kMaxInstance;
		instancingSrvDesc.Buffer.StructureByteStride = sizeof(ParticleForGpu);

		// SRVを作成するDescriptorHeapの場所を決める
		instancingSrvHandleCPU_ = texture_->GetCPUDescriptorHandle(DirectXCommon::GetInsTance()->GetSrvDescriptorHeap2(), descriptorSizeSRV, index);
		instancingSrvHandleGPU_ = texture_->GetGPUDescriptorHandle(DirectXCommon::GetInsTance()->GetSrvDescriptorHeap2(), descriptorSizeSRV, index);
		// SRVの生成
		DirectXCommon::GetInsTance()->GetDevice()->CreateShaderResourceView(instancingResource.Get(), &instancingSrvDesc, instancingSrvHandleCPU_);

		Particles::CreatePso();
		Particles::CreateVertexResource();
		Particles::CreateMaterialResource();
		Particles::CreateWVPResource();

		std::random_device seedGenerator;
		std::mt19937 randomEngine(seedGenerator());

		// 位置と速度を[-1,1]でランダムに初期化
		for (uint32_t index_ = 0; index_ < kMaxInstance; ++index_) {
			instancingData_[index_].WVP = MakeIndentity4x4();
			instancingData_[index_].World = MakeIndentity4x4();
			instancingData_[index_].color = { 1.0f, 1.0f, 1.0f, 1.0f };
			particles[index_] = MakeNewParticle(randomEngine);
			instancingData_[index_].color = particles[index_].color;
			particles[index_].transform.translate = pos;
		}

	}

	void Particles::Update() {
	}

	void Particles::Draw(Camera* camera_, uint32_t index) {
		uint32_t numInstance = 0;
		for (uint32_t index_ = 0; index_ < kMaxInstance; ++index_) {
			if (particles[index_].lifeTime <= particles[index_].currentTime) {
				continue;
			}

			Matrix4x4 worldMatrix = MakeAffineMatrix(particles[index].transform.scale, particles[index].transform.rotate, particles[index].transform.translate);
			Matrix4x4 worldViewMatrix = Multiply(worldMatrix, Multiply(worldMatrix, Multiply(camera_->viewMatrix, camera_->projectionMatrix)));
			particles[index].transform.translate.x += particles[index].velocity.x * kDeltaTime;
			particles[index].transform.translate.y += particles[index].velocity.y * kDeltaTime;
			particles[index].transform.translate.z += particles[index].velocity.z * kDeltaTime;
			particles[index].currentTime += kDeltaTime;
			instancingData_[index].World = worldMatrix;
			instancingData_[index].WVP = Multiply(worldMatrix, Multiply(camera_->viewMatrix, camera_->projectionMatrix));
			instancingData_[index].color = particles[index].color;
			/*float alpha = 1.0f - (particles[index].currentTime / particles[index].lifeTime);
			instancingData_[numInstance].color.w = alpha;*/
			++numInstance;
		}

		Matrix4x4 uvtransformMatrix = MakeScaleMatrix(uvTransform.scale);
		uvtransformMatrix = Multiply(uvtransformMatrix, MakeRotateZMatrix(uvTransform.rotate.z));
		uvtransformMatrix = Multiply(uvtransformMatrix, MakeTranslateMatrix(uvTransform.translate));
		materialData->uvTransform = uvtransformMatrix;

		ID3D12DescriptorHeap* descriptorHeaps[] = { DirectXCommon::GetInsTance()->GetSrvDescriptorHeap2().Get()}; // Use your actual descriptor heap
		DirectXCommon::GetInsTance()->GetCommandList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

		// DirectXCommon::GetInsTance()を設定。PSOに設定しているけど別途設定が必要
		DirectXCommon::GetInsTance()->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());
		DirectXCommon::GetInsTance()->GetCommandList()->SetPipelineState(graphicsPipelineState.Get());

		// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
		DirectXCommon::GetInsTance()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// コマンドを積む
		DirectXCommon::GetInsTance()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView); // VBVを設定
		// マテリアルCBufferの場所を設定
		DirectXCommon::GetInsTance()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
		// TransformationMatrixCBufferの場所を設定
		DirectXCommon::GetInsTance()->GetCommandList()->SetGraphicsRootDescriptorTable(1, instancingSrvHandleGPU_);
		// SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である。
		DirectXCommon::GetInsTance()->GetCommandList()->SetGraphicsRootDescriptorTable(2, texture_->GetTextureSRVHandleGPU(index));

		DirectXCommon::GetInsTance()->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), numInstance, 0, 0);
	}

	void Particles::Release() {
	}

	void Particles::CreateVertexResource() {
		// 頂点用のリソースを作る。
		vertexResource = CreateBufferResource(DirectXCommon::GetInsTance()->GetDevice(), sizeof(VertexData) * modelData.vertices.size()).Get();

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
		materialResource = CreateBufferResource(DirectXCommon::GetInsTance()->GetDevice(), sizeof(Material));
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
		wvpResource = CreateBufferResource(DirectXCommon::GetInsTance()->GetDevice(), sizeof(TransformationMatrix));

		// 書き込むためのアドレスを取得
		wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));

		// 単位行列を書き込んでおく
		wvpData->WVP = MakeIndentity4x4();
	}

	void Particles::CreatePso()
	{
		// dxcCompilerを初期化
		IDxcUtils* dxcUtils = nullptr;
		IDxcCompiler3* dxcCompiler = nullptr;
		HRESULT hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
		assert(SUCCEEDED(hr));
		hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
		assert(SUCCEEDED(hr));

		// 現時点でincludeしないが、includeに対応するための設定を行っておく
		IDxcIncludeHandler* includeHandler = nullptr;
		hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
		assert(SUCCEEDED(hr));

		// RootSignature作成
		D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
		descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		// particle用
		D3D12_DESCRIPTOR_RANGE descriptorRangeForInstancing[1] = {};
		descriptorRangeForInstancing[0].BaseShaderRegister = 0;
		descriptorRangeForInstancing[0].NumDescriptors = 1;
		descriptorRangeForInstancing[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		descriptorRangeForInstancing[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		// RootParameter作成。複数設定できるので配列。今回は結果1つだけなので長さ1の配列
		D3D12_ROOT_PARAMETER rootParameters[4] = {};
		rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		rootParameters[0].Descriptor.ShaderRegister = 0;

		rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
		rootParameters[1].DescriptorTable.pDescriptorRanges = descriptorRangeForInstancing;
		rootParameters[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForInstancing);

		rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // Descriptortableを使う
		rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
		rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRangeForInstancing; // Tableの中身の配列を指定
		rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForInstancing); // Tableで利用する数

		rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // CBVを使う
		rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
		rootParameters[3].Descriptor.ShaderRegister = 1; // レジスタ番号1

		descriptionRootSignature.pParameters = rootParameters; // ルートパラメータ配列へのポインタ
		descriptionRootSignature.NumParameters = _countof(rootParameters); // 配列の長さ

		// Samplerの設定
		D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
		staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // バイナリフィルタ
		staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // 0~1の範囲外をリピート
		staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; // 比較しない
		staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX; // ありったけのMipmapを使う
		staticSamplers[0].ShaderRegister = 0; // レジスタ番号0を使う
		staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う

		descriptionRootSignature.pStaticSamplers = staticSamplers;
		descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

		// シリアライズしてバイナリにする
		Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
		hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
		if (FAILED(hr)) {
			Convert::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
			assert(false);
		}
		// バイナリを元に生成
		hr = DirectXCommon::GetInsTance()->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
		assert(SUCCEEDED(hr));

		// InputLayout
		D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
		// InputLayout
		inputElementDescs[0].SemanticName = "POSITION";
		inputElementDescs[0].SemanticIndex = 0;
		inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		inputElementDescs[1].SemanticName = "TEXCOORD";
		inputElementDescs[1].SemanticIndex = 0;
		inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		inputElementDescs[2].SemanticName = "NORMAL";
		inputElementDescs[2].SemanticIndex = 0;
		inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
		inputLayoutDesc.pInputElementDescs = inputElementDescs;
		inputLayoutDesc.NumElements = _countof(inputElementDescs);

		// BlendStateの設定
		D3D12_BLEND_DESC blendDesc = {};
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		// RasiterzerStateの設定
		D3D12_RASTERIZER_DESC rasterizerDesc{};
		// 裏面(時計回り)を表示しない
		rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
		// 三角形の中を塗りつぶす
		rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

		// Shaderをコンパイルする
		IDxcBlob* vertexShaderBlob = Convert::CompileShader(L"resources/Shaders/Particle.VS.hlsl", L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
		assert(vertexShaderBlob != nullptr);

		IDxcBlob* pixelShaderBlob = Convert::CompileShader(L"resources/Shaders/Particle.PS.hlsl", L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
		assert(pixelShaderBlob != nullptr);

		// DepthStencilStateの設定
		D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
		// Depthの機能を有効化する
		depthStencilDesc.DepthEnable = true;
		// 書き込む
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;;
		// 比較関数はLessEqual。つまり、近ければ描画される
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

		// PSO生成
		D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
		graphicsPipelineStateDesc.pRootSignature = rootSignature.Get();// RootSignature
		graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;// InputLayout
		graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize() };// VertexShader
		graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize() };// PixelShader
		graphicsPipelineStateDesc.BlendState = blendDesc;// BrendState
		graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;// RasterizerState
		//書き込むRTVの情報
		graphicsPipelineStateDesc.NumRenderTargets = 1;
		graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		// 利用するトロポジ（形状）のタイプ。三角形
		graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		// どのように画面に色を打ち込むの設定
		graphicsPipelineStateDesc.SampleDesc.Count = 1;
		graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		// DepthStencilの設定
		graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
		graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		// 実際に生成
		hr = DirectXCommon::GetInsTance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState));
		assert(SUCCEEDED(hr));
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

	Particle Particles::MakeNewParticle(std::mt19937& randomEngine)
	{
		std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);
		std::uniform_real_distribution<float> distColor(0.0f, 1.0f);
		std::uniform_real_distribution<float> distTime(1.0f, 3.0f);
		Particle particle;
		// 位置と速度を[-1,1]でランダムに初期化
		particle.transform.scale = { 1.0f,1.0f,1.0f };
		particle.transform.rotate = { 0,0,0 };
		particle.transform.translate = { distribution(randomEngine), distribution(randomEngine), distribution(randomEngine) };
		particle.velocity = { distribution(randomEngine), distribution(randomEngine), distribution(randomEngine) };
		particle.color = { distColor(randomEngine), distColor(randomEngine), distColor(randomEngine), 1.0f };
		particle.lifeTime = distTime(randomEngine);
		particle.currentTime = 0;
		return particle;
	}
}

