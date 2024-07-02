#pragma once
#include <TextureManager.h>
#include <Model.h>
#include <Input.h>

class Player {
public:
	void Initialize();
	void Update();
	void Draw(Camera* camera_, uint32_t index);
private:
	Input* input_ = Input::GetInsTance();
	WorldTransform worldtransform_;
	EulerTransform transform_;
	std::unique_ptr<Model> model_;
private:
	float speed = 0.1f;
};