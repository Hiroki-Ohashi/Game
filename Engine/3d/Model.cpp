#include "Model.h"
#include "imgui.h"
#include <numbers>

/// <summary>
/// Model.cpp
/// 3Dのオブジェクト生成ソースファイル
/// </summary>
namespace Engine
{
	Model::~Model()
	{
	}

	void Model::Initialize(const std::string& filename, EulerTransform transform_) {
		// モデル読み込み
		const std::wstring filePathW = Convert::ConvertString(filename);;
		if (filePathW.ends_with(L".obj")) {
			modelData = texture_->LoadObjFile("resources", filename);
		}
		else {
			modelData = texture_->LoadModelFile("resources", filename);
		}

		//DirectX::ScratchImage mipImages2 = texture_->LoadTexture(modelData.material.textureFilePath);

		Model::CreatePso();
		Model::CreateVertexResource();
		Model::CreateMaterialResource();
		Model::CreateWVPResource();

		light_->Initialize();

		cameraResource = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(Camera));
		cameraResource->Map(0, nullptr, reinterpret_cast<void**>(&camera_));

		worldTransform_.translate = transform_.translate;
		worldTransform_.scale = transform_.scale;
		worldTransform_.rotate = transform_.rotate;
		worldTransform_.UpdateMatrix();

		uvTransform = { {1.0f, 1.0f, 1.0f},{0.0f, 0.0f, 0.0f},{0.0f, 0.0f, 0.0f}, };
	}

	void Model::Update() {

		/*if (ImGui::TreeNode("Model")) {
			ImGui::DragFloat3("Rotate", &worldTransform_.rotate.x, 0.01f);
			ImGui::DragFloat3("Transform", &worldTransform_.translate.x, 0.01f);
			ImGui::DragFloat3("Scale", &worldTransform_.scale.x, 0.01f);
			ImGui::TreePop();
		}*/

		worldTransform_.UpdateMatrix();
	}

	void Model::Draw(Camera* camera, uint32_t index) {

		//wvpData->World = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
		//wvpData->World = Multiply(wvpData->World, *camera->transformationMatrixData);
		//wvpData->WVP = wvpData->World;

		camera_.worldPosition = { camera->cameraTransform.translate.x, camera->cameraTransform.translate.y, camera->cameraTransform.translate.z };
		light_->Update();
		worldTransform_.TransferMatrix(wvpData, camera);
		worldTransform_.UpdateMatrix();

		Matrix4x4 uvtransformMatrix = MakeScaleMatrix(uvTransform.scale);
		uvtransformMatrix = Multiply(uvtransformMatrix, MakeRotateZMatrix(uvTransform.rotate.z));
		uvtransformMatrix = Multiply(uvtransformMatrix, MakeTranslateMatrix(uvTransform.translate));
		materialData->uvTransform = uvtransformMatrix;

		DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());
		DirectXCommon::GetInstance()->GetCommandList()->SetPipelineState(graphicsPipelineState.Get());
		// コマンドを積む
		DirectXCommon::GetInstance()->GetCommandList()->IASetVertexBuffers(0, 1, &modelData.vertexBufferView); // VBVを設定
		// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
		DirectXCommon::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		// マテリアルCBufferの場所を設定
		DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource.Get()->GetGPUVirtualAddress());
		// TransformationMatrixCBufferの場所を設定
		DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
		DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(3, light_->GetDirectionalLightResource()->GetGPUVirtualAddress());
		DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(4, cameraResource->GetGPUVirtualAddress());
		// SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である。
		DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootDescriptorTable(2, srvManager_->GetGPUDescriptorHandle(index));
		// 描画(DrawCall/ドローコール)
		DirectXCommon::GetInstance()->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);

		/*if (ImGui::TreeNode("Model")) {
			ImGui::SliderAngle("Rotate.y ", &worldTransform_.rotate.y);
			ImGui::DragFloat3("Transform", &worldTransform_.translate.x, 0.01f, -10.0f, 10.0f);

			ImGui::DragFloat2("UVTransform", &uvTransform.translate.x, 0.01f, -10.0f, 10.0f);
			ImGui::DragFloat2("UVScale", &uvTransform.scale.x, 0.01f, -10.0f, 10.0f);
			ImGui::SliderAngle("UVRotate", &uvTransform.rotate.z);
			ImGui::TreePop();
		}*/

		/*if (ImGui::TreeNode("Light")) {
			ImGui::SliderFloat3("Light Direction", &directionalLightData.direction.x, -1.0f, 1.0f);
			directionalLightData.direction = Normalize(directionalLightData.direction);
			ImGui::SliderFloat4("light color", &directionalLightData.color.x, 0.0f, 1.0f);
			ImGui::SliderFloat("Intensity", &directionalLightData.intensity, 0.0f, 1.0f);
			ImGui::TreePop();
		}*/
	}

	void Model::CreateVertexResource() {
		// 頂点用のリソースを作る。
		modelData.vertexResource = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(VertexData) * modelData.vertices.size());

		// リソースの先頭のアドレスから使う
		modelData.vertexBufferView.BufferLocation = modelData.vertexResource->GetGPUVirtualAddress();
		// 使用するリソースのサイズは頂点3つ分のサイズ
		modelData.vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
		// 1頂点あたりのサイズ
		modelData.vertexBufferView.StrideInBytes = sizeof(VertexData);

		// 頂点リソースにデータを書き込む
		vertexData = nullptr;

		// 書き込むためのアドレスを取得
		modelData.vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
		// 頂点データをリソースにコピー
		std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());
		// 書き込みが終了した後にアンマップ
		modelData.vertexResource->Unmap(0, nullptr);
	}

	void Model::CreateMaterialResource() {
		// マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
		materialResource = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(Material));
		// マテリアルにデータを書き込む
		materialData = nullptr;
		// 書き込むためのアドレスを取得
		materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
		// 白を設定
		materialData->color = { 1.0f, 1.0f, 1.0f, 1.0f };

		materialData->uvTransform = MakeIndentity4x4();

		materialData->enableLighting = true;

		materialData->shininess = 70.0f;

		// 書き込みが終了した後にアンマップ
		materialResource->Unmap(0, nullptr);
	}

	void Model::CreateWVPResource() {
		// WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
		wvpResource = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(TransformationMatrix));

		// 書き込むためのアドレスを取得
		wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));

		// 単位行列を書き込んでおく
		wvpData->WVP = MakeIndentity4x4();

		// 書き込みが終了した後にアンマップ
		wvpResource->Unmap(0, nullptr);
	}


	void Model::CreatePso()
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

		//DescriptorRange
		D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
		descriptorRange[0].BaseShaderRegister = 0;//0から始まる
		descriptorRange[0].NumDescriptors = 1;//数は1つ
		descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
		descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算

		// RootParameter作成。複数設定できるので配列。今回は結果1つだけなので長さ1の配列
		D3D12_ROOT_PARAMETER rootParameters[5] = {};
		rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // CBVを使う
		rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
		rootParameters[0].Descriptor.ShaderRegister = 0; // レジスタ番号0とバインド

		rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // CBVを使う
		rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // VertexShaderで使う
		rootParameters[1].Descriptor.ShaderRegister = 0;; // レジスタ番号0とバインド

		rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // Descriptortableを使う
		rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
		rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange; // Tableの中身の配列を指定
		rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange); // Tableで利用する数

		rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // CBVを使う
		rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
		rootParameters[3].Descriptor.ShaderRegister = 1; // レジスタ番号1

		rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // CBVを使う
		rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // pixelshaderを使う
		rootParameters[4].Descriptor.ShaderRegister = 2; // レジスタ番号2を使う

		descriptionRootSignature.pParameters = rootParameters; // ルートパラメータ配列へのポインタ
		descriptionRootSignature.NumParameters = _countof(rootParameters); // 配列の長さ

		// Samplerの設定
		D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
		staticSamplers[0].Filter = D3D12_FILTER_ANISOTROPIC;
		staticSamplers[0].MaxAnisotropy = 8; // 8x 異方性フィルタリング
		staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // 0~1の範囲外をリピート
		staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; // 比較しない
		staticSamplers[0].MinLOD = 0.0f;
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
			if (errorBlob) {
				Convert::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
			}
			assert(false);
		}
		// バイナリを元に生成
		hr = DirectXCommon::GetInstance()->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
		assert(SUCCEEDED(hr));

		// InputLayout
		D3D12_INPUT_ELEMENT_DESC inputElementDescs[4] = {};
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

		inputElementDescs[3].SemanticName = "WPOSITION";
		inputElementDescs[3].SemanticIndex = 0;
		inputElementDescs[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputElementDescs[3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
		inputLayoutDesc.pInputElementDescs = inputElementDescs;
		inputLayoutDesc.NumElements = _countof(inputElementDescs);

		// BlendStateの設定
		D3D12_BLEND_DESC blendDesc{};
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		blendDesc.RenderTarget[0].BlendEnable = true;
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

		// RasterizerState
		D3D12_RASTERIZER_DESC rasterizerDesc{};
		// 裏面(時計回り)を表示しない
		rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
		// 三角形の中を塗りつぶす
		rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

		// Shaderをコンパイルする
		IDxcBlob* vertexShaderBlob = Convert::CompileShader(L"resources/Shaders/Object3d.VS.hlsl", L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
		assert(vertexShaderBlob != nullptr);

		IDxcBlob* pixelShaderBlob = Convert::CompileShader(L"resources/Shaders/Object3d.PS.hlsl", L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
		assert(pixelShaderBlob != nullptr);

		// DepthStencilStateの設定
		D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
		// Depthの機能を有効化する
		depthStencilDesc.DepthEnable = true;
		// 書き込む
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
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
		hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState));
		assert(SUCCEEDED(hr));
	}

	Microsoft::WRL::ComPtr<ID3D12Resource> Model::CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInbytes)
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
		[[maybe_unused]] HRESULT hr_ = device->CreateCommittedResource(
			&uploadHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&ResourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&Resource));

		assert(SUCCEEDED(hr_));

		return Resource;
	}
}
