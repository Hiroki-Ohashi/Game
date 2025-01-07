#pragma once
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <cstdint>

#include "WinApp.h"
#include "Function.h"
#include "MathFunction.h"
#include "DirectXCommon.h"
#include "TextureManager.h"
#include "Mesh.h"
#include "Camera.h"

/// <summary>
/// Sprite.h
/// 2Dのスプライト生成のヘッダーファイル
/// </summary>
/// 
namespace Engine
{
	// Spriteクラス
	class Sprite {
	public:
		// デストラクタ
		~Sprite();

		// 初期化処理
		void Initialize(Vector2 pos, Vector2 scale, uint32_t index);
		// 更新処理
		void Update();
		// 描画処理
		void Draw();
		// 解放処理
		void Release();

		// Getter
		Material* GetMaterialDataSprite() { return materialDataSprite; }
		const Vector2& GetAnchorPoint() const { return anchorPoint; }
		Vector2 GetSize() { return { transformSprite.scale.x,  transformSprite.scale.y }; }
		Vector2 GetPos() { return { transformSprite.translate.x,  transformSprite.translate.y }; }
		Vector2 GetTextureLeftTop() { return textureLeftTop; }

		// Setter
		void SetAnchorPoint(const Vector2 anchorPoint_) { this->anchorPoint = anchorPoint_; }
		void SetPosition(Vector2 pos_) {
			transformSprite.translate.x = pos_.x;
			transformSprite.translate.y = pos_.y;
		}
		void SetSize(Vector2 size) { 
			transformSprite.scale.x = size.x;
			transformSprite.scale.y = size.y;
		}
		void SetTextureLeftTop(const Vector2 textureLeftTop_) { this->textureLeftTop = textureLeftTop_; }
		void SetAlpha(float alpha) { materialDataSprite->color.w = alpha; }
		float GetAlpha() { return materialDataSprite->color.w; }

		// シーン遷移処理
		void FadeIn(float speed);
		void FadeOut(float speed);

		// テクスチャのサイズをスプライトに合わせる
		void AdjustTextureSize();

	private:
		// vertex生成
		void CreateVertexResourceSprite();
		// material作成
		void CreateMaterialResourceSprite();
		// wvp作成
		void CreateTransformationMatrixResourceSprite();
		// PSO作成
		void CreatePso();

		// Resource生成
		Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInbytes);

	private:
		// シングルトン呼び出し
		DirectXCommon* dir_ = DirectXCommon::GetInsTance();
		TextureManager* texture_ = TextureManager::GetInstance();

		// Resource
		Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceSprite;
		Microsoft::WRL::ComPtr<ID3D12Resource> materialResourceSprite;
		Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResourceSprite;
		Microsoft::WRL::ComPtr<ID3D12Resource> indexResourceSprite;

		// BufferView
		D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};
		D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite{};

		// PSO
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;

		// Data
		VertexData* vertexDataSprite;
		Material* materialDataSprite;
		TransformationMatrix* transformationMatrixDataSprite;
		uint32_t* indexDataSprite;

		// Transform
		EulerTransform transformSprite;
		EulerTransform uvTransformSprite;

		// シーン遷移条件
		bool isSprite = false;

		// テクスチャの格納
		uint32_t textureIndex;

		// アンカーポイント
		Vector2 anchorPoint = { 0.0f, 0.0f };

		// 画像の左上の座標
		Vector2 textureLeftTop = { 0.0f,0.0f };
		// テクスチャの切り出しサイズ
		Vector2 textureSize = { 100.0f, 100.0f };

	};
}