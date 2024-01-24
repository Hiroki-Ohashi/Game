#pragma once

#include "DirectXCommon.h"
#include "MathFunction.h"
#include "Camera.h"

struct WorldTransform {
	Vector3 scale = { 1.0f,1.0f,1.0f };
	Vector3 rotate = { 0.0f,0.0f,0.0f };
	Vector3 translate = { 0.0f,0.0f,0.0f };

	Matrix4x4 matWorld = {};
	Matrix4x4 worldMatrix = {};
	Matrix4x4 sMatWorld = {};

	const WorldTransform* parent = nullptr;

	void Initialize();

	void TransferMatrix(TransformationMatrix* wvpData, Camera* camera);

	void sTransferMatrix(Microsoft::WRL::ComPtr<ID3D12Resource>& wvpResource, Camera& camera);

	void UpdateMatrix();
};