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

void WorldTransform::UpdateMatrix() {

}