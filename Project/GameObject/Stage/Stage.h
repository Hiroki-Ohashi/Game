#pragma once
#include <WorldTransform.h>
#include <Model.h>

class Stage {
public:
	void Initialize();
	void Update();
	void Draw(Camera* camera);
private:
	TextureManager* textureManager_ = TextureManager::GetInstance();
	WorldTransform worldtransform_;
	EulerTransform transform_;

	std::unique_ptr<Model> model_;
private:
	uint32_t stageTex;
};