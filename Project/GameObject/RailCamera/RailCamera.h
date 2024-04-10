#pragma once
#include "utillity/WorldTransform.h"
#include "utillity/Camera.h"

class RailCamera {
public:
	~RailCamera();
	void Initialize(Vector3 pos, Vector3 rot);

	void Update();

	// ワールド座標を取得
	const WorldTransform& GetWorldTransform() { return worldTransform_; };

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	Camera* camera_ = nullptr;
};