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

	// 衝突属性を設定
	SetCollosionAttribute(kcollisionAttributeObject);
	// 衝突対象を自分の属性以外に設定
	SetCollisionMask(kcollisionAttributePlayer);
	SetCollisionMask(kcollisionAttributeObject);
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

OBB Object::GetOBB()
{
	OBB obb;

	// 中心座標の位置
	obb.center = GetWorldPosition();

	// 半サイズはスケールの半分
	obb.halfSize = GetHalfSize();

	// 各ローカル軸を正規化して格納
	obb.axis[0] = Normalize({ worldtransform_.matWorld.m[0][0], worldtransform_.matWorld.m[0][1], worldtransform_.matWorld.m[0][2] }); // X軸
	obb.axis[1] = Normalize({ worldtransform_.matWorld.m[1][0], worldtransform_.matWorld.m[1][1], worldtransform_.matWorld.m[1][2] }); // Y軸
	obb.axis[2] = Normalize({ worldtransform_.matWorld.m[2][0], worldtransform_.matWorld.m[2][1], worldtransform_.matWorld.m[2][2] }); // Z軸

	return obb;
}
