#include "Model.h"
#include "imgui.h"
#include <numbers>

void Model::Initialize(const std::string& filename, EulerTransform transform) {
	// モデル読み込み
	modelData = texture_->LoadModelFile("resources", filename);

	DirectX::ScratchImage mipImages2 = texture_->LoadTexture(modelData.material.textureFilePath);

	worldTransform_.translate = transform.translate;
	worldTransform_.scale = transform.scale;
	worldTransform_.rotate = transform.rotate;

	Model::CreateVertexResource();
	Model::CreateMaterialResource();
	Model::CreateWVPResource();
	Model::CreateIndexResource();
	Model::CreateDirectionalResource();

	cameraResource = CreateBufferResource(DirectXCommon::GetInsTance()->GetDevice(), sizeof(Camera));
	cameraResource->Map(0, nullptr, reinterpret_cast<void**>(&camera));
	camera.worldPosition = { 0.0f, 0.0f, 0.0f };

	uvTransform = { {1.0f, 1.0f, 1.0f},{0.0f, 0.0f, 0.0f},{0.0f, 0.0f, 0.0f}, };

	directionalLightData.color = { 1.0f, 1.0f, 1.0f, 1.0f };
	directionalLightData.direction = { 0.0f, -1.0f, 1.0f };
	directionalLightData.intensity = 1.0f;
}

void Model::Update() {

	ImGui::Begin("Model");
	ImGui::DragFloat3("Rotate", &worldTransform_.rotate.x, 0.01f, -100.0f, 100.0f);
	ImGui::End();
}

void Model::Draw(Camera* camera, uint32_t index) {

	//wvpData->World = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	//wvpData->World = Multiply(wvpData->World, *camera->transformationMatrixData);
	//wvpData->WVP = wvpData->World;

	//worldTransform_.GltfTransferMatrix(modelData, wvpData, camera);
	worldTransform_.TransferMatrix(wvpData, camera);

	Matrix4x4 uvtransformMatrix = MakeScaleMatrix(uvTransform.scale);
	uvtransformMatrix = Multiply(uvtransformMatrix, MakeRotateZMatrix(uvTransform.rotate.z));
	uvtransformMatrix = Multiply(uvtransformMatrix, MakeTranslateMatrix(uvTransform.translate));
	materialData->uvTransform = uvtransformMatrix;


	// コマンドを積む
	DirectXCommon::GetInsTance()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView); // VBVを設定
	DirectXCommon::GetInsTance()->GetCommandList()->IASetIndexBuffer(&indexBufferView); // VBVを設定
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	DirectXCommon::GetInsTance()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// マテリアルCBufferの場所を設定
	DirectXCommon::GetInsTance()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource.Get()->GetGPUVirtualAddress());
	// TransformationMatrixCBufferの場所を設定
	DirectXCommon::GetInsTance()->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
	DirectXCommon::GetInsTance()->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());
	DirectXCommon::GetInsTance()->GetCommandList()->SetGraphicsRootConstantBufferView(4, cameraResource->GetGPUVirtualAddress());
	// SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である。
	DirectXCommon::GetInsTance()->GetCommandList()->SetGraphicsRootDescriptorTable(2, texture_->GetTextureSRVHandleGPU(index));
	// 描画(DrawCall/ドローコール)
	//DirectXCommon::GetInsTance()->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);
	DirectXCommon::GetInsTance()->GetCommandList()->DrawIndexedInstanced(UINT(modelData.indices.size()), 1, 0, 0, 0);


	if (ImGui::TreeNode("Model")) {
		ImGui::DragFloat3("Rotate", &worldTransform_.rotate.x, 0.01f);
		ImGui::DragFloat3("Transform", &worldTransform_.translate.x, 0.01f);

		ImGui::DragFloat2("UVTransform", &uvTransform.translate.x, 0.01f, -10.0f, 10.0f);
		ImGui::DragFloat2("UVScale", &uvTransform.scale.x, 0.01f, -10.0f, 10.0f);
		ImGui::SliderAngle("UVRotate", &uvTransform.rotate.z);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Light")) {
		ImGui::SliderFloat3("Light Direction", &directionalLightData.direction.x, -1.0f, 1.0f);
		directionalLightData.direction = Normalize(directionalLightData.direction);
		ImGui::SliderFloat4("light color", &directionalLightData.color.x, 0.0f, 1.0f);
		ImGui::SliderFloat("Intensity", &directionalLightData.intensity, 0.0f, 1.0f);
		ImGui::TreePop();
	}
}

void Model::CreateVertexResource() {
	// 頂点用のリソースを作る。
	vertexResource = CreateBufferResource(DirectXCommon::GetInsTance()->GetDevice(), sizeof(VertexData) * modelData.vertices.size());

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

void Model::CreateMaterialResource() {
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

	materialData->shininess = 70.0f;
}

void Model::CreateWVPResource() {
	// WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	wvpResource = CreateBufferResource(DirectXCommon::GetInsTance()->GetDevice(), sizeof(TransformationMatrix));

	// 書き込むためのアドレスを取得
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));

	// 単位行列を書き込んでおく
	wvpData->WVP = MakeIndentity4x4();
}

void Model::CreateDirectionalResource()
{
	directionalLightResource = CreateBufferResource(DirectXCommon::GetInsTance()->GetDevice(), sizeof(DirectionalLight));
	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
}

void Model::CreateIndexResource()
{
	// 頂点用のリソースを作る。
	indexResource = CreateBufferResource(DirectXCommon::GetInsTance()->GetDevice(), sizeof(uint32_t) * modelData.indices.size());

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
	HRESULT hr_ = device->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&ResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&Resource));

	assert(SUCCEEDED(hr_));

	return Resource;
}