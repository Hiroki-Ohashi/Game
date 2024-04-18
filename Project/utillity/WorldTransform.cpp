#include "WorldTransform.h"

void WorldTransform::Initialize() {

}

void WorldTransform::TransferMatrix(TransformationMatrix* wvpData, Camera* camera) {
	wvpData->World = MakeAffineMatrix(scale, rotate, translate);
	wvpData->World = Multiply(wvpData->World, *camera->transformationMatrixData);
	wvpData->WVP = wvpData->World;

	/*wvpData->WVP = modelData.rootNode.localmatrix * worldMatrix * camera->transformationMatrixData;
	wvpData->World = modelData.rootNode.localmatrix * worldMatrix;*/
}

void WorldTransform::sTransferMatrix(Microsoft::WRL::ComPtr<ID3D12Resource>& wvpResource, Camera& camera)
{
	
}

void WorldTransform::GltfTransferMatrix(ModelData modelData, TransformationMatrix* wvpData, Camera* camera)
{
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