#include "Animation.h"

/// <summary>
/// Animation.cpp
/// 3Dアニメーション生成のソースファイル
/// </summary>
namespace Engine {
	AnimationModel::~AnimationModel()
	{
	}

	void AnimationModel::Initialize(const std::string& filename, EulerTransform param, Camera* camera, uint32_t index_)
	{
		modelData = texture_->LoadModelFile("resources", filename);
		DirectX::ScratchImage mipImages2 = texture_->LoadTexture(modelData.material.textureFilePath);

		animation = texture_->LoadAnimationFile("resources", filename);

		AnimationModel::CreatePso();
		AnimationModel::CreateVertexResource();
		AnimationModel::CreateMaterialResource();
		AnimationModel::CreateWVPResource();
		AnimationModel::CreateIndexResource();
		AnimationModel::CreateCameraResource(camera);

		light_->Initialize();

		cameraResource = CreateBufferResource(dir_->GetDevice(), sizeof(CameraForGpu));
		cameraResource->Map(0, nullptr, reinterpret_cast<void**>(&camera_));

		skeleton = CreateSkelton(modelData.rootNode);
		skinCluster = CreateSkinCluster(dir_->GetDevice(), skeleton, modelData, index_);

		worldTransform_.Initialize();
		worldTransform_.translate = param.translate;
		worldTransform_.scale = param.scale;
		worldTransform_.rotate = param.rotate;
		worldTransform_.UpdateMatrix();

		uvTransform = { {1.0f, 1.0f, 1.0f},{0.0f, 0.0f, 0.0f},{0.0f, 0.0f, 0.0f}, };
	}

	void AnimationModel::Update(float time)
	{
		// アニメーションの時間を進める
		animationTime += 1.0f / 60.0f;
		animationTime = fmod(animationTime, time);

		// アニメーションの更新を行って、骨ごとのLocal情報を更新する
		ApplyAnimation(skeleton, animation, animationTime);

		// すべてのJointを更新。親が若いので通常ループで処理可能
		for (Joint& joint_ : skeleton.joints) {
			joint_.localMatrix = MakeAffineMatrixQuaternion(joint_.transform.scale, joint_.transform.rotate, joint_.transform.translate);
			if (joint_.parent) { // 親がいれば親の行列をかける
				joint_.skeltonSpaceMatrix = Multiply(joint_.localMatrix, skeleton.joints[*joint_.parent].skeltonSpaceMatrix);
			}
			else { // 親がいないのでLocalMatrixとskeletonSpaceMatrixは一致する
				joint_.skeltonSpaceMatrix = joint_.localMatrix;
			}
		}

		// SkinClusterの更新
		for (size_t jointIndex = 0; jointIndex < skeleton.joints.size(); ++jointIndex) {
			assert(jointIndex < skinCluster.inverseBindPoseMatrices.size());
			skinCluster.mappedPalette[jointIndex].skeletonSpaceMatrix = Multiply(skinCluster.inverseBindPoseMatrices[jointIndex], skeleton.joints[jointIndex].skeltonSpaceMatrix);
			skinCluster.mappedPalette[jointIndex].skeletonSpaceInverseTransposeMatrix = Transpose(Inverse(skinCluster.mappedPalette[jointIndex].skeletonSpaceMatrix));
		}

		worldTransform_.UpdateMatrix();
	}

	void AnimationModel::Draw(Camera* camera, uint32_t index_, uint32_t index2)
	{

		// コマンドを積む
		// DirectXCommon::GetInsTance()を設定。PSOに設定しているけど別途設定が必要
		dir_->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());
		dir_->GetCommandList()->SetPipelineState(graphicsPipelineState.Get());
		dir_->GetCommandList()->SetGraphicsRootDescriptorTable(5, skinCluster.paletteSrvHandle.second);

		worldTransform_.AnimationTransferMatrix(skeleton, animation, wvpData, camera);
		worldTransform_.UpdateMatrix();

		camera_->worldPosition = { camera->cameraTransform.translate.x, camera->cameraTransform.translate.y, camera->cameraTransform.translate.z };

		light_->Update();

		//Matrix4x4 uvtransformMatrix = MakeScaleMatrix(uvTransform.scale);
		//uvtransformMatrix = Multiply(uvtransformMatrix, MakeRotateZMatrix(uvTransform.rotate.z));
		//uvtransformMatrix = Multiply(uvtransformMatrix, MakeTranslateMatrix(uvTransform.translate));
		//materialData->uvTransform = uvtransformMatrix;

		D3D12_VERTEX_BUFFER_VIEW vbvs[2] = {
			vertexBufferView,
			skinCluster.influenceBufferView
		};

		dir_->GetCommandList()->IASetVertexBuffers(0, 2, vbvs); // VBVを設定
		dir_->GetCommandList()->IASetIndexBuffer(&indexBufferView); // IBVを設定
		// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
		dir_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		// マテリアルCBufferの場所を設定
		dir_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource.Get()->GetGPUVirtualAddress());
		// TransformationMatrixCBufferの場所を設定
		dir_->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource.Get()->GetGPUVirtualAddress());
		dir_->GetCommandList()->SetGraphicsRootConstantBufferView(3, light_->GetDirectionalLightResource()->GetGPUVirtualAddress());
		dir_->GetCommandList()->SetGraphicsRootConstantBufferView(4, cameraResource.Get()->GetGPUVirtualAddress());
		// SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である。
		dir_->GetCommandList()->SetGraphicsRootDescriptorTable(2, srvManager_->GetGPUDescriptorHandle(index_));
		dir_->GetCommandList()->SetGraphicsRootDescriptorTable(6, srvManager_->GetGPUDescriptorHandle(index2));
		// 描画(DrawCall/ドローコール)
		//DirectXCommon::GetInsTance()->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);
		dir_->GetCommandList()->DrawIndexedInstanced(UINT(modelData.indices.size()), 1, 0, 0, 0);

		if (ImGui::TreeNode("AnimationModel")) {
			ImGui::DragFloat3("scale", &worldTransform_.scale.x, 0.01f, -50.0f, 50.0f);
			ImGui::DragFloat3("rotate", &worldTransform_.rotate.x, 0.01f, -50.0f, 50.0f);
			ImGui::DragFloat3("translate", &worldTransform_.translate.x, 0.01f, -50.0f, 50.0f);
			ImGui::TreePop();
		}
	}

	void AnimationModel::CreatePso()
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

		// 環境マップ用
		D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
		descriptorRange[0].BaseShaderRegister = 1;
		descriptorRange[0].NumDescriptors = 1;
		descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		// RootParameter作成。複数設定できるので配列。今回は結果1つだけなので長さ1の配列
		D3D12_ROOT_PARAMETER rootParameters[7] = {};
		rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		rootParameters[0].Descriptor.ShaderRegister = 0;

		rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // CBVを使う
		rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // VertexShaderで使う
		rootParameters[1].Descriptor.ShaderRegister = 0;; // レジスタ番号0とバインド

		rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // Descriptortableを使う
		rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
		rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRangeForInstancing; // Tableの中身の配列を指定
		rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForInstancing); // Tableで利用する数

		rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // CBVを使う
		rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
		rootParameters[3].Descriptor.ShaderRegister = 1; // レジスタ番号1

		rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // CBVを使う
		rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
		rootParameters[4].Descriptor.ShaderRegister = 2; // レジスタ番号2

		rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
		rootParameters[5].DescriptorTable.pDescriptorRanges = descriptorRangeForInstancing;
		rootParameters[5].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForInstancing);

		rootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		rootParameters[6].DescriptorTable.pDescriptorRanges = descriptorRange;
		rootParameters[6].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);

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
			if (errorBlob) {
				Convert::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
			}
			assert(false);
		}
		// バイナリを元に生成
		hr = dir_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
		assert(SUCCEEDED(hr));

		// InputLayout
		std::array<D3D12_INPUT_ELEMENT_DESC, 5> inputElementDescs{};
		// InputLayout
		inputElementDescs[0].SemanticName = "POSITION";
		inputElementDescs[0].SemanticIndex = 0;
		inputElementDescs[0].InputSlot = 0;
		inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		inputElementDescs[1].SemanticName = "TEXCOORD";
		inputElementDescs[1].SemanticIndex = 0;
		inputElementDescs[1].InputSlot = 0;
		inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		inputElementDescs[2].SemanticName = "NORMAL";
		inputElementDescs[2].SemanticIndex = 0;
		inputElementDescs[2].InputSlot = 0;
		inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		inputElementDescs[3].SemanticName = "WEIGHT";
		inputElementDescs[3].SemanticIndex = 0;
		inputElementDescs[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		inputElementDescs[3].InputSlot = 1;
		inputElementDescs[3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		inputElementDescs[4].SemanticName = "INDEX";
		inputElementDescs[4].SemanticIndex = 0;
		inputElementDescs[4].Format = DXGI_FORMAT_R32G32B32A32_SINT;
		inputElementDescs[4].InputSlot = 1;
		inputElementDescs[4].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
		inputLayoutDesc.pInputElementDescs = inputElementDescs.data();
		inputLayoutDesc.NumElements = static_cast<UINT>(inputElementDescs.size());

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
		IDxcBlob* vertexShaderBlob = Convert::CompileShader(L"resources/Shaders/SkinningObject3d.VS.hlsl", L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
		assert(vertexShaderBlob != nullptr);

		IDxcBlob* pixelShaderBlob = Convert::CompileShader(L"resources/Shaders/SkinningObject3d.PS.hlsl", L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
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

	void AnimationModel::CreateVertexResource()
	{
		// 頂点用のリソースを作る。
		vertexResource = CreateBufferResource(dir_->GetDevice(), sizeof(VertexData) * modelData.vertices.size());

		// リソースの先頭のアドレスから使う
		vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
		// 使用するリソースのサイズは頂点3つ分のサイズ
		vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
		// 1頂点あたりのサイズ
		vertexBufferView.StrideInBytes = sizeof(VertexData);

		// 頂点リソースにデータを書き込む
		vertexData = nullptr;

		// 書き込むためのアドレスを取得
		vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
		// 頂点データをリソースにコピー
		std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());
	}

	void AnimationModel::CreateMaterialResource()
	{
		// マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
		materialResource = CreateBufferResource(dir_->GetDevice(), sizeof(Material));
		// マテリアルにデータを書き込む
		materialData = nullptr;
		// 書き込むためのアドレスを取得
		materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
		// 白を設定
		materialData->color = { 1.0f, 1.0f, 1.0f, 1.0f };

		materialData->uvTransform = MakeIndentity4x4();

		materialData->enableLighting = true;

		materialData->shininess = 70.0f;
	}

	void AnimationModel::CreateWVPResource()
	{
		// WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
		wvpResource = CreateBufferResource(dir_->GetDevice(), sizeof(TransformationMatrix));

		// 書き込むためのアドレスを取得
		wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));

		// 単位行列を書き込んでおく
		wvpData->WVP = MakeIndentity4x4();
	}

	void AnimationModel::CreateIndexResource()
	{
		// 頂点用のリソースを作る。
		indexResource = CreateBufferResource(dir_->GetDevice(), sizeof(uint32_t) * modelData.indices.size());

		// リソースの先頭のアドレスから使う
		indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
		// 使用するリソースのサイズは頂点3つ分のサイズ
		indexBufferView.SizeInBytes = UINT(sizeof(uint32_t) * modelData.indices.size());
		// 1頂点あたりのサイズ
		indexBufferView.Format = DXGI_FORMAT_R32_UINT;

		// 書き込むためのアドレスを取得
		indexResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedIndex));

		// 頂点データをリソースにコピー
		std::memcpy(mappedIndex, modelData.indices.data(), sizeof(uint32_t) * modelData.indices.size());
	}

	void AnimationModel::CreateCameraResource(Camera* camera)
	{

		cameraResource = CreateBufferResource(dir_->GetDevice(), sizeof(CameraForGpu));
		cameraResource->Map(0, nullptr, reinterpret_cast<void**>(&camera_));
		camera_->worldPosition = { camera->cameraTransform.translate.x, camera->cameraTransform.translate.y, camera->cameraTransform.translate.z };
	}

	Skeleton AnimationModel::CreateSkelton(const Node& rootNode)
	{
		Skeleton skeleton_;
		skeleton_.root = CreateJoint(rootNode, {}, skeleton_.joints);

		// 名前とIndexのマッピングを行いアクセスしやすくする
		for (const Joint& joint_ : skeleton_.joints) {
			skeleton_.jointmap.emplace(joint_.name, joint_.index);
		}

		return skeleton_;
	}

	int32_t AnimationModel::CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints)
	{
		Joint joint_;

		joint_.name = node.name;
		joint_.localMatrix = node.localmatrix;
		joint_.skeltonSpaceMatrix = MakeIndentity4x4();
		joint_.transform = node.transform;
		joint_.index = int32_t(joints.size());
		joint_.parent = parent;

		joints.push_back(joint_);
		for (const Node& child : node.children) {
			// 子jointを作成し、そのIndexを登録
			int32_t childIndex = CreateJoint(child, joint_.index, joints);
			joints[joint_.index].children.push_back(childIndex);
		}

		// 自身のIndexを返す
		return joint_.index;
	}

	SkinCluster AnimationModel::CreateSkinCluster(const Microsoft::WRL::ComPtr<ID3D12Device>& device, const Skeleton& skeleton_, const ModelData& modelData_, uint32_t index_)
	{
		SkinCluster skinCluster_;

		// palette用のResourceを確保
		skinCluster_.paletteResource = CreateBufferResource(device, sizeof(WellForGPU) * skeleton_.joints.size());
		WellForGPU* mappedPalette = nullptr;
		skinCluster_.paletteResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedPalette));
		skinCluster_.mappedPalette = { mappedPalette, skeleton_.joints.size() }; // spanを使ってアクセスするようにする
		skinCluster_.paletteSrvHandle.first = srvManager_->GetCPUDescriptorHandle(10 + index_);
		skinCluster_.paletteSrvHandle.second = srvManager_->GetGPUDescriptorHandle(10 + index_);

		// palette用のsrvを作成
		D3D12_SHADER_RESOURCE_VIEW_DESC paletteSrvDesc{};
		paletteSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
		paletteSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		paletteSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		paletteSrvDesc.Buffer.FirstElement = 0;
		paletteSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		paletteSrvDesc.Buffer.NumElements = UINT(skeleton_.joints.size());
		paletteSrvDesc.Buffer.StructureByteStride = sizeof(WellForGPU);
		device->CreateShaderResourceView(skinCluster_.paletteResource.Get(), &paletteSrvDesc, skinCluster_.paletteSrvHandle.first);

		// influence用のResourceを確保
		skinCluster_.influenceResource = CreateBufferResource(device, sizeof(VertexInfluence) * modelData_.vertices.size());
		VertexInfluence* mappedInfluence = nullptr;
		skinCluster_.influenceResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedInfluence));
		std::memset(mappedInfluence, 0, sizeof(VertexInfluence) * modelData_.vertices.size());
		skinCluster_.mappedInfluence = { mappedInfluence, modelData_.vertices.size() };

		// influence用のVBVを作成
		skinCluster_.influenceBufferView.BufferLocation = skinCluster_.influenceResource->GetGPUVirtualAddress();
		skinCluster_.influenceBufferView.SizeInBytes = UINT(sizeof(VertexInfluence) * modelData_.vertices.size());
		skinCluster_.influenceBufferView.StrideInBytes = sizeof(VertexInfluence);

		// InverseBindPoseMatrixの保存領域を作成
		skinCluster_.inverseBindPoseMatrices.resize(skeleton_.joints.size());
		std::generate(skinCluster_.inverseBindPoseMatrices.begin(), skinCluster_.inverseBindPoseMatrices.end(), MakeIndentity4x4);

		// ModelDataのSkinCluster情報を解析してInfluenceの中身を埋める
		for (const auto& JointWeight : modelData_.skinClusterData) {// modelのSkinClusterの情報を更新
			auto it = skeleton_.jointmap.find(JointWeight.first);// jointWeight.firstはjoint名なので、skeltonに対象となるjointが含まれているか判断
			if (it == skeleton_.jointmap.end()) {// そんな名前のJointは存在しないので次に回す
				continue;
			}

			// (*it).secondにはjointのindexが入っているので、該当のindexのinverseBindPoseMatrixを代入
			skinCluster_.inverseBindPoseMatrices[(*it).second] = JointWeight.second.inverseBindPoseMatrix;
			for (const auto& vertexWeight : JointWeight.second.vertexWeights) {
				auto& currentInfluence = skinCluster_.mappedInfluence[vertexWeight.vertexIndex];// 該当のvertexIndexのinfluence情報を参照しておく
				for (uint32_t int_ = 0; int_ < kNumMaxInfluence; ++int_) {// 空いてるところに入れる
					if (currentInfluence.weights[int_] == 0.0f) {// weight == 0が空いてる状態なので、その場所にweightとjointのindexを代入
						currentInfluence.weights[int_] = vertexWeight.weight;
						currentInfluence.jointIndices[int_] = (*it).second;
						break;
					}
				}
			}
		}

		return skinCluster_;
	}

	Microsoft::WRL::ComPtr<ID3D12Resource> AnimationModel::CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInbytes)
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

	void AnimationModel::ApplyAnimation(Skeleton& skeleton_, const Animation& animation_, float animationTime_)
	{
		for (Joint& joint_ : skeleton_.joints) {
			// 対象のJointのAnimationがあれば、値の適用を行う
			if (auto it = animation_.nodeAnimations.find(joint_.name); it != animation_.nodeAnimations.end()) {
				const NodeAnimation& rootNodeAnimation = (*it).second;
				joint_.transform.translate = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime_);
				joint_.transform.rotate = CalculateValueRotate(rootNodeAnimation.rotate.keyframes, animationTime_);
				joint_.transform.scale = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime_);
			}
		}
	}
}
