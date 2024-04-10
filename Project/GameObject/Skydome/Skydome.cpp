#include "Skydome.h"

Skydome::~Skydome()
{
}

void Skydome::Initialize()
{
	transform = { { 10000.0f,10000.0f,10000.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	model_ = std::make_unique<Model>();
	model_->Initialize("skydome.obj", transform);
}

void Skydome::Update()
{
}

void Skydome::Draw(Camera* camera, uint32_t index)
{
	model_->Draw(camera, index);
}
