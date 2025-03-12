#include "Stage.h"

void Stage::Initialize()
{
	transform_ = { {40000.0f,1.0f,40000.0f},{0.0f,0.0f,0.0f},{0.0f,-100.0f,0.0f} };
	uvTransform_ = { {300.0f,300.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	model_ = std::make_unique<Model>();
	model_->Initialize("plane.obj", transform_);
	model_->SetUVTransform(uvTransform_);

	worldtransform_.scale = transform_.scale;
	worldtransform_.rotate = transform_.rotate;
	worldtransform_.translate = transform_.translate;
	worldtransform_.UpdateMatrix();

	stageTex = textureManager_->Load("resources/stage.png");
}

void Stage::Update()
{
	model_->SetWorldTransform(worldtransform_);
	model_->SetUVTransform(uvTransform_);

	if (ImGui::TreeNode("Stage")) {
		ImGui::DragFloat3("Scale", &worldtransform_.scale.x, 0.1f);
		ImGui::DragFloat3("Rotate.y ", &worldtransform_.rotate.x, 0.1f);
		ImGui::DragFloat3("Transform", &worldtransform_.translate.x, 0.1f);
		ImGui::DragFloat3("uv", &uvTransform_.scale.x, 1.0f);
		ImGui::TreePop();
	}

	worldtransform_.UpdateMatrix();
}

void Stage::Draw(Camera* camera)
{
	model_->Draw(camera, stageTex);
}
