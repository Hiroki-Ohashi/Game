#pragma once
#include <Camera.h>
#include <Model.h>
using namespace Engine;
/// <summary>
/// Skydome.h
/// Skydome生成のヘッダーファイル
/// </summary>

// SkyDomeクラス
class Skydome {
public:
	// 初期化処理
	void Initialize();
	// 描画処理
	void Draw(Camera* camera);
private:
	// シングルトン呼び出し
	TextureManager* textureManager_ = TextureManager::GetInstance();

	// モデル
	EulerTransform transform;
	std::unique_ptr<Model> model_ = nullptr;

	// テクスチャ
	uint32_t skydome;
};
