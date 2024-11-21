#include "Light.h"

/// <summary>
/// Light.h
/// 光源生成のヘッダーファイル
/// </summary>
namespace Engine
{
	Light* Light::GetInstance()
	{
		static Light instance;
		return &instance;
	}

	void Light::Initialize()
	{
		Light::CreateDirectionalResource();

		directionalLightData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
		directionalLightData->direction = { 0.0f, -1.0f, 1.0f };
		directionalLightData->intensity = 1.0f;
	}

	void Light::Update()
	{
		directionalLightData->direction = Normalize(directionalLightData->direction);

		/*if (ImGui::TreeNode("Light")) {
			ImGui::SliderFloat3("Light Direction", &directionalLightData->direction.x, -1.0f, 1.0f);
			ImGui::SliderFloat4("light color", &directionalLightData->color.x, 0.0f, 1.0f);
			ImGui::SliderFloat("Intensity", &directionalLightData->intensity, 0.0f, 1.0f);
			ImGui::TreePop();
		}*/
	}

	void Light::CreateDirectionalResource()
	{
		directionalLightResource = CreateBufferResource(dir_->GetDevice(), sizeof(DirectionalLight));
		directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
	}

	Microsoft::WRL::ComPtr<ID3D12Resource> Light::CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInbytes)
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
		[[maybe_unused]] HRESULT hr = device->CreateCommittedResource(
			&uploadHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&ResourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&Resource));

		assert(SUCCEEDED(hr));

		return Resource;
	}
}
