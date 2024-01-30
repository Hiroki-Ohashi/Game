#include "Sprite.h"
#include "imgui.h"


void Sprite::Initialize(Vector2 pos, Vector2 scale, float index){

	Sprite::CreateVertexResourceSprite(pos, scale);
	Sprite::CreateMaterialResourceSprite();
	Sprite::CreateTransformationMatrixResourceSprite();

	transformSprite = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	uvTransformSprite = {{1.0f, 1.0f, 1.0f},{0.0f, 0.0f, 0.0f},{0.0f, 0.0f, 0.0f},};

	// SpriteはLightingしないのでfalseを設定
	materialDataSprite->enableLighting = false;

	// 白を設定
	materialDataSprite->color = { 1.0f, 1.0f, 1.0f, index };
}

void Sprite::Update(float alpha){
	materialDataSprite->color.w = alpha;
}

void Sprite::Draw(uint32_t index){

	transformationMatrixDataSprite->World = MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
	Matrix4x4 viewMatrixSprite = MakeIndentity4x4();
	Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, 0.0f, float(WinApp::GetInsTance()->GetKClientWidth()), float(WinApp::GetInsTance()->GetKClientHeight()), 0.0f, 100.0f);
	Matrix4x4 worldViewProjectionMatrixSprite = Multiply(transformationMatrixDataSprite->World, Multiply(viewMatrixSprite, projectionMatrixSprite));
	transformationMatrixDataSprite->WVP = worldViewProjectionMatrixSprite;

	Matrix4x4 uvtransformMatrix = MakeScaleMatrix(uvTransformSprite.scale);
	uvtransformMatrix = Multiply(uvtransformMatrix, MakeRotateZMatrix(uvTransformSprite.rotate.z));
	uvtransformMatrix = Multiply(uvtransformMatrix, MakeTranslateMatrix(uvTransformSprite.translate));
	materialDataSprite->uvTransform = uvtransformMatrix;

	// コマンドを積む
	dir_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferViewSprite); // VBVを設定
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	dir_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// Spriteの描画。変更が必要なものだけ変更する
	dir_->GetCommandList()->IASetIndexBuffer(&indexBufferViewSprite);
	// マテリアルCBufferの場所を設定
	dir_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResourceSprite->GetGPUVirtualAddress());
	// TransformationMatrixCBufferの場所を設定
	dir_->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceSprite->GetGPUVirtualAddress());
	// SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である。
	dir_->GetCommandList()->SetGraphicsRootDescriptorTable(2, texture_->GetTextureSRVHandleGPU(index));
	// 描画(DrawCall/ドローコール)
	dir_->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
	
	/*if (ImGui::TreeNode("Sprite")) {
		ImGui::DragFloat2("Transform", &transformSprite.translate.x, 0.1f, -1000.0f, 1000.0f);
		ImGui::DragFloat4("aaa", &materialDataSprite->color.x, 0.1f, -10.0f, 10.0f);
		ImGui::DragFloat2("UVTransform", &uvTransformSprite.translate.x, 0.01f, -10.0f, 10.0f);
		ImGui::DragFloat2("UVScale", &uvTransformSprite.scale.x, 0.01f, -10.0f, 10.0f);
		ImGui::SliderAngle("UVRotate", &uvTransformSprite.rotate.z);
		ImGui::TreePop();
	}*/
}

void Sprite::Release(){
}

void Sprite::CreateVertexResourceSprite(Vector2 pos, Vector2 scale){
	// Sprite用の頂点リソースを作る
	vertexResourceSprite = CreateBufferResource(dir_->GetDevice(), sizeof(VertexData) * 4);

	// リソースの先頭のアドレスから使う
	vertexBufferViewSprite.BufferLocation = vertexResourceSprite->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferViewSprite.SizeInBytes = sizeof(VertexData) * 4;
	// 1頂点あたりのサイズ
	vertexBufferViewSprite.StrideInBytes = sizeof(VertexData);

	vertexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite));

	// 三角形
	vertexDataSprite[0].position = { pos.x, pos.y + scale.y, 0.0f, 1.0f }; // 左下
	vertexDataSprite[0].texcoord = { 0.0f, 1.0f };
	vertexDataSprite[0].normal = { 0.0f, 0.0f, -1.0f };

	vertexDataSprite[1].position = { pos.x, pos.y, 0.0f, 1.0f }; // 左上
	vertexDataSprite[1].texcoord = { 0.0f, 0.0f };
	vertexDataSprite[1].normal = { 0.0f, 0.0f, -1.0f };

	vertexDataSprite[2].position = { pos.x + scale.x, pos.y + scale.y, 0.0f, 1.0f }; // 右下
	vertexDataSprite[2].texcoord = { 1.0f, 1.0f };
	vertexDataSprite[2].normal = { 0.0f, 0.0f, -1.0f };

	vertexDataSprite[3].position = { pos.x + scale.x, pos.y, 0.0f, 1.0f }; // 右上
	vertexDataSprite[3].texcoord = { 1.0f, 0.0f };
	vertexDataSprite[3].normal = { 0.0f, 0.0f, -1.0f };

	// index
	indexResourceSprite = CreateBufferResource(dir_->GetDevice(), sizeof(uint32_t) * 6);
	// リソースの先頭のアドレスから使う
	indexBufferViewSprite.BufferLocation = indexResourceSprite->GetGPUVirtualAddress();
	// 使用するリソースのサイズはindex6つ分のサイズ
	indexBufferViewSprite.SizeInBytes = sizeof(uint32_t) * 6;
	// indexはuint32_tとする
	indexBufferViewSprite.Format = DXGI_FORMAT_R32_UINT;

	indexDataSprite = nullptr;

	indexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite));

	indexDataSprite[0] = 0;
	indexDataSprite[1] = 1;
	indexDataSprite[2] = 2;
	indexDataSprite[3] = 1;
	indexDataSprite[4] = 3;
	indexDataSprite[5] = 2;
}

void Sprite::CreateMaterialResourceSprite(){
	// マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	materialResourceSprite = CreateBufferResource(dir_->GetDevice(), sizeof(Material));
	// マテリアルにデータを書き込む
	materialDataSprite = nullptr;
	// 書き込むためのアドレスを取得
	materialResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&materialDataSprite));

	materialDataSprite->uvTransform = MakeIndentity4x4();
}


void Sprite::CreateTransformationMatrixResourceSprite(){
	// Sprite用のTransformationMatrix用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	transformationMatrixResourceSprite = CreateBufferResource(dir_->GetDevice(), sizeof(TransformationMatrix));

	// 書き込むためのアドレスを取得
	transformationMatrixResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSprite));

	// 単位行列を書き込んでおく
	transformationMatrixDataSprite->WVP = MakeIndentity4x4();
}

Microsoft::WRL::ComPtr<ID3D12Resource> Sprite::CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInbytes)
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