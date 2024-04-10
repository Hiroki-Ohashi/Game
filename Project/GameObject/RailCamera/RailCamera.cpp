#include "GameObject/RailCamera/RailCamera.h"

RailCamera::~RailCamera()
{
	delete camera_;
}

void RailCamera::Initialize(Vector3 pos, Vector3 rot) {

	camera_ = new Camera();
	camera_->Initialize();

	// ワールドトランスフォームの初期設定
	camera_->cameraTransform.translate.x = pos.x;
	camera_->cameraTransform.translate.y = pos.y;
	camera_->cameraTransform.translate.z = pos.z - 10;

	camera_->cameraTransform.rotate = rot;
}

void RailCamera::Update() {
	camera_->Update();
}