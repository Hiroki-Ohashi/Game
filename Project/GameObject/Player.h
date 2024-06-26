#pragma once
#include <TextureManager.h>
#include <Model.h>

class Player {
public:
	void Init();
	void Update();
	void Draw(Camera* camera_, uint32_t index);
private:
	TextureManager* texure_ = TextureManager::GetInstance();
	WorldTransform worldtransform_;
	EulerTransform transform_;

	std::unique_ptr<Model> model_;
};