#include "Sphere.h"
#define _USE_MATH_DEFINES
#include "math.h"
#include "imgui.h"
#include <numbers>

/// <summary>
/// Sphere.cpp
/// 球体生成のソースファイル
/// </summary>
namespace Engine
{
	Sphere::~Sphere()
	{
	}

	void Sphere::Initialize() {

		Sphere::CreateVertexResourceSphere();
		Sphere::CreateMaterialResourceSphere();
		Sphere::CreateTransformationMatrixResourceSphere();
		Sphere::CreateDirectionalResource();

		worldTransform_.scale = { 0.5f,0.5f,0.5f };

		transformSphere = { {0.5f,0.5f,0.5f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
		uvTransformSphere = { {1.0f, 1.0f, 1.0f},{0.0f, 0.0f, 0.0f},{0.0f, 0.0f, 0.0f}, };

		worldTransform_.translate = transformSphere.translate;

		cameraResource = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(Camera));
		cameraResource->Map(0, nullptr, reinterpret_cast<void**>(&camera_));

		directionalLightData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
		directionalLightData->direction = { 0.0f, -1.0f, 1.0f };
		directionalLightData->intensity = 1.0f;
	}

	void Sphere::Update() {
	}

	void Sphere::Draw(Camera* camera, uint32_t index) {

		worldTransform_.rotate.y += 0.02f;

		worldTransform_.TransferMatrix(wvpResourceDataSphere, camera);
		camera_.worldPosition = { camera->cameraTransform.translate.x, camera->cameraTransform.translate.y, camera->cameraTransform.translate.z };
		directionalLightData->direction = Normalize(directionalLightData->direction);

		Matrix4x4 uvtransformMatrix = MakeScaleMatrix(uvTransformSphere.scale);
		uvtransformMatrix = Multiply(uvtransformMatrix, MakeRotateZMatrix(uvTransformSphere.rotate.z));
		uvtransformMatrix = Multiply(uvtransformMatrix, MakeTranslateMatrix(uvTransformSphere.translate));
		materialDataSphere->uvTransform = uvtransformMatrix;

		// Spriteの描画。変更が必要なものだけ変更する
		DirectXCommon::GetInstance()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferViewSphere); // VBVを設定
		// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
		DirectXCommon::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		// マテリアルCBufferの場所を設定
		DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResourceSphere.Get()->GetGPUVirtualAddress());
		// TransformationMatrixCBufferの場所を設定
		DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResourceSphere->GetGPUVirtualAddress());
		DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());
		DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(4, cameraResource.Get()->GetGPUVirtualAddress());
		// SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である。
		DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootDescriptorTable(2, srvManager_->GetGPUDescriptorHandle(index));
		// 描画(DrawCall/ドローコール)
		DirectXCommon::GetInstance()->GetCommandList()->DrawInstanced(vertexIndex, 1, 0, 0);

		if (ImGui::TreeNode("Sphere")) {
			ImGui::DragFloat3("Scale", &worldTransform_.scale.x, 0.01f, -10.0f, 10.0f);
			ImGui::DragFloat3("Rotate", &worldTransform_.rotate.x, 0.01f, -10.0f, 10.0f);
			ImGui::DragFloat3("Transform", &worldTransform_.translate.x, 0.01f, -10.0f, 10.0f);

			ImGui::DragFloat2("UVTransform", &uvTransformSphere.translate.x, 0.01f, -10.0f, 10.0f);
			ImGui::DragFloat2("UVScale", &uvTransformSphere.scale.x, 0.01f, -10.0f, 10.0f);
			ImGui::SliderAngle("UVRotate", &uvTransformSphere.rotate.z);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("SphereLight")) {
			ImGui::SliderFloat3("Light Direction", &directionalLightData->direction.x, -1.0f, 1.0f);
			directionalLightData->direction = Normalize(directionalLightData->direction);
			ImGui::SliderFloat4("light color", &directionalLightData->color.x, 0.0f, 1.0f);
			ImGui::SliderFloat("Intensity", &directionalLightData->intensity, 0.0f, 1.0f);
			ImGui::TreePop();
		}
	}

	void Sphere::Release() {
	}

	void Sphere::CreateVertexResourceSphere() {

		// 頂点リソースを作る
		vertexResourceSphere = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(VertexData) * vertexIndex);

		// リソースの先頭のアドレスから使う
		vertexBufferViewSphere.BufferLocation = vertexResourceSphere->GetGPUVirtualAddress();
		// 使用するリソースのサイズは頂点6つ分のサイズ
		vertexBufferViewSphere.SizeInBytes = sizeof(VertexData) * vertexIndex;
		// 1頂点あたりのサイズ
		vertexBufferViewSphere.StrideInBytes = sizeof(VertexData);

		vertexResourceSphere->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSphere));

		// 経度1つ分の角度
		const float kLonEvery = 2.0f * float(std::numbers::pi) / float(kSubdivision);

		// 緯度1つ分の角度
		const float kLatEvery = float(std::numbers::pi) / float(kSubdivision);

		// 緯度の方向に分割
		for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
			float lat = float(-std::numbers::pi) / 2.0f + kLatEvery * latIndex;

			// 経度の方向に分割しながら線を書く
			for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
				int32_t start = (latIndex * kSubdivision + lonIndex) * 6;
				float lon = lonIndex * kLonEvery;

				// 頂点にデータを入力する
				// 基準点a
				vertexDataSphere[start].position.x = cos(lat) * cos(lon);
				vertexDataSphere[start].position.y = sin(lat);
				vertexDataSphere[start].position.z = cos(lat) * sin(lon);
				vertexDataSphere[start].position.w = 1.0f;

				vertexDataSphere[start].texcoord = { float(lonIndex) / float(kSubdivision) ,1.0f - float(latIndex) / float(kSubdivision) };

				vertexDataSphere[start].normal.x = vertexDataSphere[start].position.x;
				vertexDataSphere[start].normal.y = vertexDataSphere[start].position.y;
				vertexDataSphere[start].normal.z = vertexDataSphere[start].position.z;

				// 基準点b
				vertexDataSphere[start + 1].position.x = cos(lat + kLatEvery) * cos(lon);
				vertexDataSphere[start + 1].position.y = sin(lat + kLatEvery);
				vertexDataSphere[start + 1].position.z = cos(lat + kLatEvery) * sin(lon);
				vertexDataSphere[start + 1].position.w = 1.0f;

				vertexDataSphere[start + 1].texcoord = { vertexDataSphere[start].texcoord.x,vertexDataSphere[start].texcoord.y - (1.0f / kSubdivision) };

				vertexDataSphere[start + 1].normal.x = vertexDataSphere[start + 1].position.x;
				vertexDataSphere[start + 1].normal.y = vertexDataSphere[start + 1].position.y;
				vertexDataSphere[start + 1].normal.z = vertexDataSphere[start + 1].position.z;

				// 基準点c
				vertexDataSphere[start + 2].position.x = cos(lat) * cos(lon + kLonEvery);
				vertexDataSphere[start + 2].position.y = sin(lat);
				vertexDataSphere[start + 2].position.z = cos(lat) * sin(lon + kLonEvery);
				vertexDataSphere[start + 2].position.w = 1.0f;

				vertexDataSphere[start + 2].texcoord = { vertexDataSphere[start].texcoord.x + (1.0f / (float)kSubdivision),vertexDataSphere[start].texcoord.y };

				vertexDataSphere[start + 2].normal.x = vertexDataSphere[start + 2].position.x;
				vertexDataSphere[start + 2].normal.y = vertexDataSphere[start + 2].position.y;
				vertexDataSphere[start + 2].normal.z = vertexDataSphere[start + 2].position.z;

				// 基準点b
				vertexDataSphere[start + 3].position.x = cos(lat + kLatEvery) * cos(lon);
				vertexDataSphere[start + 3].position.y = sin(lat + kLatEvery);
				vertexDataSphere[start + 3].position.z = cos(lat + kLatEvery) * sin(lon);
				vertexDataSphere[start + 3].position.w = 1.0f;

				vertexDataSphere[start + 3].texcoord = { vertexDataSphere[start].texcoord.x,vertexDataSphere[start].texcoord.y - (1.0f / (float)kSubdivision) };

				vertexDataSphere[start + 3].normal.x = vertexDataSphere[start + 3].position.x;
				vertexDataSphere[start + 3].normal.y = vertexDataSphere[start + 3].position.y;
				vertexDataSphere[start + 3].normal.z = vertexDataSphere[start + 3].position.z;

				// 基準点d
				vertexDataSphere[start + 4].position.x = cos(lat + kLatEvery) * cos(lon + kLonEvery);
				vertexDataSphere[start + 4].position.y = sin(lat + kLatEvery);
				vertexDataSphere[start + 4].position.z = cos(lat + kLatEvery) * sin(lon + kLonEvery);
				vertexDataSphere[start + 4].position.w = 1.0f;

				vertexDataSphere[start + 4].texcoord = { vertexDataSphere[start].texcoord.x + (1.0f / (float)kSubdivision),vertexDataSphere[start].texcoord.y - (1.0f / (float)kSubdivision) };

				vertexDataSphere[start + 4].normal.x = vertexDataSphere[start + 4].position.x;
				vertexDataSphere[start + 4].normal.y = vertexDataSphere[start + 4].position.y;
				vertexDataSphere[start + 4].normal.z = vertexDataSphere[start + 4].position.z;

				// 基準点c
				vertexDataSphere[start + 5].position.x = cos(lat) * cos(lon + kLonEvery);
				vertexDataSphere[start + 5].position.y = sin(lat);
				vertexDataSphere[start + 5].position.z = cos(lat) * sin(lon + kLonEvery);
				vertexDataSphere[start + 5].position.w = 1.0f;

				vertexDataSphere[start + 5].texcoord = { vertexDataSphere[start].texcoord.x + (1.0f / (float)kSubdivision),vertexDataSphere[start].texcoord.y };

				vertexDataSphere[start + 5].normal.x = vertexDataSphere[start + 5].position.x;
				vertexDataSphere[start + 5].normal.y = vertexDataSphere[start + 5].position.y;
				vertexDataSphere[start + 5].normal.z = vertexDataSphere[start + 5].position.z;

			}
		}
	}

	void Sphere::CreateMaterialResourceSphere() {
		// マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
		materialResourceSphere = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(Material));
		// マテリアルにデータを書き込む
		materialDataSphere = nullptr;
		// 書き込むためのアドレスを取得
		materialResourceSphere->Map(0, nullptr, reinterpret_cast<void**>(&materialDataSphere));
		// 白を設定
		materialDataSphere->color = { 1.0f, 1.0f, 1.0f, 1.0f };
		// SphereはLightingするのでtrueを設定
		materialDataSphere->enableLighting = true;

		materialDataSphere->uvTransform = MakeIndentity4x4();

		materialDataSphere->shininess = 70.0f;
	}

	void Sphere::CreateTransformationMatrixResourceSphere() {
		// Sprite用のTransformationMatrix用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
		wvpResourceSphere = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(TransformationMatrix));

		// 書き込むためのアドレスを取得
		wvpResourceSphere->Map(0, nullptr, reinterpret_cast<void**>(&wvpResourceDataSphere));

		// 単位行列を書き込んでおく
		wvpResourceDataSphere->WVP = MakeIndentity4x4();
		wvpResourceDataSphere->World = MakeIndentity4x4();
	}

	void Sphere::CreateDirectionalResource()
	{
		directionalLightResource = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(DirectionalLight));
		directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
	}

	Microsoft::WRL::ComPtr<ID3D12Resource> Sphere::CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInbytes)
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
