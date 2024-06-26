#pragma once
#include "Function.h"
#include "MathFunction.h"
#include "Camera.h"
#include "WorldTransform.h"
#include "TextureManager.h"
#include "Light.h"

class AnimationModel {
public:
	void Initialize(const std::string& filename, EulerTransform transform);
	void Update(float time);
	void Draw(Camera* camera, uint32_t index, uint32_t index2);

	void SetTranslate(Vector3 translate) const { translate = worldTransform_.translate; }
	void SetScale(Vector3 scale) { scale = worldTransform_.scale; }
	void SetRotate(Vector3 rotate) { rotate = worldTransform_.rotate; }

private:
	void CreatePso();
	void CreateVertexResource();
	void CreateMaterialResource();
	void CreateWVPResource();
	void CreateIndexResource();
	void CreateDirectionalResource();

	Skeleton CreateSkelton(const Node& rootNode);
	int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);
	SkinCluster CreateSkinCluster(const Microsoft::WRL::ComPtr<ID3D12Device>& device, const Skeleton& skeleton, const ModelData& modelData, const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize);
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInbytes);
	void ApplyAnimation(Skeleton& skeleton, const Animation& animation, float animationTime);

private:
	DirectXCommon* dir_ = DirectXCommon::GetInsTance();
	TextureManager* texture_ = TextureManager::GetInstance();
	Light* light_ = Light::GetInstance();
	CameraForGpu camera_;

	WorldTransform worldTransform_;
	EulerTransform transform;
	EulerTransform uvTransform;

	Animation animation;
	float animationTime = 0.0f;

	Skeleton skeleton;
	SkinCluster skinCluster;
	Joint joint;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;

private:
	ModelData modelData;
	uint32_t* mappedIndex;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource;

	VertexData* vertexData;
	Material* materialData;
	TransformationMatrix* wvpData;
	DirectionalLight* directionalLightData;
};