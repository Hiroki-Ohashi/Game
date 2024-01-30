#include "Skydome.h"

Skydome::~Skydome()
{
	delete model_;
}

void Skydome::Initialize()
{
	transform = { { 100.0f,100.0f,100.0f},{0.0f,0.0f,0.0f},{0.01f,0.01f,0.01f} };

	model_ = new Model();
	model_->Initialize("skydome.obj", transform);
}

void Skydome::Update()
{
}

void Skydome::Draw(Camera* camera, uint32_t index)
{
	model_->Draw(camera, index);
}
