#include "Object.h"

void Object::Initialize(const std::string& filename, EulerTransform transform)
{
	transform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	model_ = std::make_unique<Model>();
	model_->Initialize(filename, transform);

	worldtransform_.Initialize();
	worldtransform_.scale = transform_.scale;
	worldtransform_.rotate = transform_.rotate;
	worldtransform_.translate = transform_.translate;
	worldtransform_.UpdateMatrix();
}

void Object::Update()
{
	model_->SetWorldTransform(worldtransform_);
	worldtransform_.UpdateMatrix();

	if (ImGui::TreeNode("Object")) {
		ImGui::DragFloat3("Scale ", &worldtransform_.scale.x, 0.01f);
		ImGui::DragFloat3("Rotate.y ", &worldtransform_.rotate.x, 0.01f);
		ImGui::DragFloat3("Transform", &worldtransform_.translate.x, 0.01f);
		ImGui::TreePop();
	}
}

void Object::Draw(Camera* camera, uint32_t index)
{
	model_->Draw(camera, index);
}

void Object::OnCollision()
{
}

Vector3 Object::GetWorldPosition() const
{
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldtransform_.matWorld.m[3][0];
	worldPos.y = worldtransform_.matWorld.m[3][1];
	worldPos.z = worldtransform_.matWorld.m[3][2];

	return worldPos;
}
