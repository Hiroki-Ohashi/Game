#include "WorldTransform.h"

/// <summary>
/// WorldTransform.cpp
/// ワールド座標生成のソースファイル
/// </summary>

void WorldTransform::Initialize() {

}

void WorldTransform::TransferMatrix(TransformationMatrix* wvpData, Camera* camera) {
	wvpData->World = Multiply(worldMatrix, *camera->transformationMatrixData);
	wvpData->WVP = wvpData->World;
}

void WorldTransform::GltfTransferMatrix(ModelData modelData, TransformationMatrix* wvpData, Camera* camera)
{
	wvpData->WVP = Multiply(modelData.rootNode.localmatrix, Multiply(worldMatrix, *camera->transformationMatrixData));
	wvpData->World = Multiply(modelData.rootNode.localmatrix, worldMatrix);
}

void WorldTransform::AnimationTransferMatrix(Skeleton skeleton, Animation animation, TransformationMatrix* wvpData, Camera* camera)
{
	animation;
	wvpData->WVP = Multiply(worldMatrix, *camera->transformationMatrixData);
	for (Joint& joint : skeleton.joints) {
		wvpData->World = Multiply(joint.skeltonSpaceMatrix, worldMatrix);
	}
	wvpData->WorldInverseTranspose = Transpose(Inverse(wvpData->WVP));
}

void WorldTransform::UpdateMatrix() {
	matWorld = MakeAffineMatrix(scale, rotate, translate);
	worldMatrix = MakeAffineMatrix(scale, rotate, translate);

	if (parent) {
		matWorld = Multiply(matWorld, parent->matWorld);
	}
}