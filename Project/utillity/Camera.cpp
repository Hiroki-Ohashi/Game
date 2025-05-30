#include "Camera.h"
#include "imgui.h"

/// <summary>
/// Camera.cpp
/// カメラ生成のソースファイル
/// </summary>

void Camera::Initialize(){
	cameraTransform = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
}

void Camera::Update(){
	cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
	viewMatrix = Inverse(cameraMatrix);
	projectionMatrix = MakePerspectiveMatrix(fov, float(winapp_->GetKClientWidth()) / float(winapp_->GetKClientHeight()), 0.1f, 1000000.0f);

	sMatView = MakeIndentity4x4();
	sMatProjection = MakeOrthographicMatrix(0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 100.0f);

	worldMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f });
	worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
	transformationMatrixData = &worldViewProjectionMatrix;

	if (ImGui::TreeNode("Camera")) {
		ImGui::DragFloat3("Camera Transform", &cameraTransform.translate.x, 0.1f);
		ImGui::DragFloat3("Camera Rotate", &cameraTransform.rotate.x, 0.01f);
		ImGui::TreePop();
	}
}

void Camera::Draw(){}
