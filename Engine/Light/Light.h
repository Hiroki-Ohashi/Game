#pragma once
#include <MathFunction.h>
#include <DirectXCommon.h>

class Light {
public:
	static Light* GetInstance();

	void Initialize();
	void Update();

	Microsoft::WRL::ComPtr<ID3D12Resource> GetDirectionalLightResource() {return directionalLightResource;}
private:
	void CreateDirectionalResource();
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInbytes);
private:
	DirectXCommon* dir_ = DirectXCommon::GetInsTance();
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource;
	DirectionalLight* directionalLightData;
};