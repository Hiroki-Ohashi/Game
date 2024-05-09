#include "WorldTransform.h"

void WorldTransform::Initialize() {

}

void WorldTransform::TransferMatrix(TransformationMatrix* wvpData, Camera* camera) {
	wvpData->World = MakeAffineMatrix(scale, rotate, translate);
	wvpData->World = Multiply(wvpData->World, *camera->transformationMatrixData);
	wvpData->WVP = wvpData->World;
}

void WorldTransform::sTransferMatrix(Microsoft::WRL::ComPtr<ID3D12Resource>& wvpResource, Camera& camera)
{
	
}

void WorldTransform::GltfTransferMatrix(ModelData modelData, TransformationMatrix* wvpData, Camera* camera)
{
	wvpData->WVP = Multiply(modelData.rootNode.localmatrix, Multiply(worldMatrix, *camera->transformationMatrixData));
	wvpData->World = Multiply(modelData.rootNode.localmatrix, worldMatrix);
}

void WorldTransform::AnimationTransferMatrix(ModelData modelData, Animation animation, TransformationMatrix* wvpData, Camera* camera)
{
	animationTime += 1.0f / 60.0f; // 時刻を進める。
	animationTime = std::fmod(animationTime, animation.duration);// リピート再生
	NodeAnimation& rootNodeAnimation = animation.nodeAnimations[modelData.rootNode.name];
	Vector3 translate = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime);
	Quaternion rotate = CalculateValueRotate(rootNodeAnimation.rotate.keyframes, animationTime);
	Vector3 scale = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime);

	modelData.rootNode.localmatrix = MakeAffineMatrixQuaternion(scale, rotate, translate);

	wvpData->WVP = Multiply(modelData.rootNode.localmatrix, Multiply(worldMatrix, *camera->transformationMatrixData));
	wvpData->World = Multiply(modelData.rootNode.localmatrix, worldMatrix);
}

void WorldTransform::UpdateMatrix() {
	matWorld = MakeAffineMatrix(scale, rotate, translate);
	worldMatrix = MakeAffineMatrix(scale, rotate, translate);

	if (parent) {
		matWorld = Multiply(matWorld, parent->matWorld);
	}
}