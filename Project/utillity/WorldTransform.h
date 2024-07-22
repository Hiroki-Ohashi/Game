#pragma once

#include "DirectXCommon.h"
#include "MathFunction.h"
#include "Camera.h"

struct WorldTransform {

	Vector3 scale = {1.0f, 1.0f, 1.0f};
	Vector3 rotate = { 0.0f, 0.0f, 0.0f };
	Vector3 translate = { 0.0f, 0.0f, 0.0f };

	Matrix4x4 matWorld = {};
	Matrix4x4 worldMatrix = {};
	Matrix4x4 sMatWorld = {};

	Matrix4x4 worldViewProjectionMatrix;
	Matrix4x4* transformationMatrixData;

	const WorldTransform* parent = nullptr;

	void Initialize();

	void TransferMatrix(TransformationMatrix* wvpData, Camera* camera);

	void sTransferMatrix(Microsoft::WRL::ComPtr<ID3D12Resource>& wvpResource, Camera& camera);

	void GltfTransferMatrix(ModelData modelData, TransformationMatrix* wvpData, Camera* camera);
	//void AnimationTransferMatrix(ModelData modelData, Animation animation, TransformationMatrix* wvpData, Camera* camera);
	void AnimationTransferMatrix(Skeleton skeleton, Animation animation, TransformationMatrix* wvpData, Camera* camera);

	void UpdateMatrix();
};