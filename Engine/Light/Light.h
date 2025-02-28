#pragma once
#include <MathFunction.h>
#include <DirectXCommon.h>

// <summary>
/// Light.h
/// 高原生成のヘッダーファイル
/// </summary>
namespace Engine
{
	// Lightクラス
	class Light {
	public:
		// シングルトン
		static Light* GetInstance();
		// 初期化処理
		void Initialize();
		// 更新処理
		void Update();

		// Getter
		Microsoft::WRL::ComPtr<ID3D12Resource> GetDirectionalLightResource() { return directionalLightResource; }
	private:
		// リソース生成
		void CreateDirectionalResource();
		Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInbytes);
	private:
		// シングルトン飛び出し
		DirectXCommon* dir_ = DirectXCommon::GetInstance();

		// リソース
		Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource;
		DirectionalLight* directionalLightData;
	};
}