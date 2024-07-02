#include "Stage.h"

void Stage::Initialize()
{
	transform_ = { {30.0f,1.0f,10000.0f},{0.0f,0.0f,0.0f},{0.0f,-15.0f,0.0f} };

	model_ = std::make_unique<Model>();
	model_->Initialize("cube.obj", transform_);

	worldtransform_.scale = transform_.scale;
	worldtransform_.rotate = transform_.rotate;
	worldtransform_.translate = transform_.translate;
	worldtransform_.UpdateMatrix();

	stageTex = textureManager_->Load("resources/kusa.png");
}

void Stage::Update()
{
	model_->SetWorldTransform(worldtransform_);

	if (ImGui::TreeNode("Stage")) {
		ImGui::DragFloat3("Scale", &worldtransform_.scale.x, 0.1f);
		ImGui::DragFloat3("Rotate.y ", &worldtransform_.rotate.x, 0.1f);
		ImGui::DragFloat3("Transform", &worldtransform_.translate.x, 0.1f);
		ImGui::TreePop();
	}
}

void Stage::Draw(Camera* camera)
{
	model_->Draw(camera, stageTex);
}
