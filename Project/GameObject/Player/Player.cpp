#include "Player.h"

void Player::Initialize()
{
	transform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	model_ = std::make_unique<Model>();
	model_->Initialize("cube.obj", transform_);

	worldtransform_.scale = transform_.scale;
	worldtransform_.rotate = transform_.rotate;
	worldtransform_.translate = transform_.translate;
	worldtransform_.UpdateMatrix();
}

void Player::Update()
{
	// キーボード移動処理
	if (input_->PushKey(DIK_W)) {
		worldtransform_.translate.y += speed;
	}
	if (input_->PushKey(DIK_S)) {
		worldtransform_.translate.y -= speed;
	}
	if (input_->PushKey(DIK_A)) {
		worldtransform_.translate.x -= speed;
	}
	if (input_->PushKey(DIK_D)) {
		worldtransform_.translate.x += speed;
	}

	if (ImGui::TreeNode("Player")) {
		ImGui::DragFloat3("Rotate.y ", &worldtransform_.rotate.x, 0.01f);
		ImGui::DragFloat3("Transform", &worldtransform_.translate.x, 0.01f);

		//ImGui::DragFloat2("UVTransform", &worldtransform_.translate.x, 0.01f, -10.0f, 10.0f);
		//ImGui::DragFloat2("UVScale", &worldtransform_.scale.x, 0.01f, -10.0f, 10.0f);
		//ImGui::SliderAngle("UVRotate", &worldtransform_.rotate.z);
		ImGui::TreePop();
	}
	worldtransform_.UpdateMatrix();
	model_->SetWorldTransform(worldtransform_);
}

void Player::Draw(Camera* camera_, uint32_t index)
{
	model_->Draw(camera_, index);
}
