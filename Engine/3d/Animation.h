#pragma once
#include "Function.h"
#include "MathFunction.h"
#include "Camera.h"
#include "WorldTransform.h"

#include "Model.h"

class AnimationModel {
public:
	void Initialize(const std::string& filename, EulerTransform transform);
	void Update(float time);
	void Draw(Camera* camera, uint32_t index);
private:
	void CreatePso();

	Skeleton CreateSkelton(const Node& rootNode);
	int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);
	SkinCluster CreateSkinCluster(const Microsoft::WRL::ComPtr<ID3D12Device>& device, const Skeleton& skeleton, const ModelData& modelData, const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize);
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInbytes);
	void ApplyAnimation(Skeleton& skeleton, const Animation& animation, float animationTime);

private:
	DirectXCommon* dir_ = DirectXCommon::GetInsTance();
	TextureManager* texture_ = TextureManager::GetInstance();
	std::unique_ptr<Model> model_ = nullptr;

	WorldTransform worldTransform_;
	EulerTransform transform;

	Animation animation;
	float animationTime = 0.0f;

	Skeleton skeleton;
	SkinCluster skinCluster;
	Joint joint;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;
};