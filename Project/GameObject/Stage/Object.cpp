#include "Object.h"

void Object::Initialize(const std::string& filename, EulerTransform transform)
{
	model_ = std::make_unique<Model>();
	model_->Initialize(filename, transform);

	worldtransform_.Initialize();
	worldtransform_.scale = transform.scale;
	worldtransform_.rotate = transform.rotate;
	worldtransform_.translate = transform.translate;
	worldtransform_.UpdateMatrix();
}

void Object::Update()
{
	model_->SetWorldTransform(worldtransform_);
	worldtransform_.UpdateMatrix();
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
