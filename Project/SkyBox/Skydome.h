#pragma once
#include <Camera.h>
#include <Model.h>

class Skydome {
public:
	void Initialize();
	void Draw(Camera* camera);
private:
	// モデル
	EulerTransform transform;
	std::unique_ptr<Model> model_ = nullptr;
	TextureManager* textureManager_ = TextureManager::GetInstance();

	uint32_t skydome;
};
