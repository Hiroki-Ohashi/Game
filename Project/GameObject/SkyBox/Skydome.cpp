#include "Skydome.h"

/// <summary>
/// Skydome.h
/// Skydome生成のソースファイル
/// </summary>

void Skydome::Initialize()
{
	transform = { { 40000.0f,40000.0f,40000.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	model_ = std::make_unique<Model>();
	model_->Initialize("skydome.obj", transform);

	skydome = textureManager_->Load("resources/skydome.png");

	model_->SetLight(false);
}

void Skydome::Draw(Camera* camera)
{
	model_->Draw(camera, skydome);
}