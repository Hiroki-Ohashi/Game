#pragma once
#include <WorldTransform.h>
#include <Model.h>
#include "Collider.h"
using namespace Engine;

class Object : public Collider {
public:
	void Initialize(const std::string& filename, EulerTransform transform);
	void Update();
	void Draw(Camera* camera, uint32_t index);

	// 当たり判定処理
	void OnCollision() override;
	Vector3 GetWorldPosition() const override;

	// Getter
	Vector3 GetScale() { return worldtransform_.scale; }
	Vector3 GetPosition() { return worldtransform_.translate; }
	Vector3 GetHalfSize() const {
		return {
			worldtransform_.scale.x / 2.0f,
			worldtransform_.scale.y / 2.0f,
			worldtransform_.scale.z / 2.0f
		};
	}

	// AABBの最小座標を取得
	Vector3 GetAABBMin() const {
		return {
			worldtransform_.translate.x - (worldtransform_.scale.x / 1.0f),
			worldtransform_.translate.y - (worldtransform_.scale.y / 1.0f),
			worldtransform_.translate.z - (worldtransform_.scale.z / 1.0f)
		};
	}

	// AABBの最大座標を取得
	Vector3 GetAABBMax() const {
		return {
			worldtransform_.translate.x + (worldtransform_.scale.x / 1.0f),
			worldtransform_.translate.y + (worldtransform_.scale.y / 1.0f),
			worldtransform_.translate.z + (worldtransform_.scale.z / 1.0f)
		};
	}

	// Setter
	void SetWorldTransform(WorldTransform worldtransform) { worldtransform_ = worldtransform; }
	void SetUVTransform(EulerTransform uvtransform) { uvTransform_ = uvtransform; }
	void SetPosition(Vector3 pos) { worldtransform_.translate = pos; }
	void SetRotation(Vector3 rot) { worldtransform_.rotate = rot; }
	void SetScale(Vector3 scale) { 
		worldtransform_.scale = scale;
		model_->SetWorldTransform(worldtransform_);
		worldtransform_.UpdateMatrix();
	}
	void SetLight(bool isLight) { model_->SetLight(isLight); }
private:
	WorldTransform worldtransform_;
	EulerTransform transform_;
	EulerTransform uvTransform_;

	std::unique_ptr<Model> model_;
private:
};