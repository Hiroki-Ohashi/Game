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
	void Update(float time, Vector3 pos, Vector3 rotate);
	void Draw(Camera* camera, uint32_t index);
private:
	void CreatePso();
	void CreateVertexResource();
	void CreateMaterialResource();
	void CreateWVPResource();
	void CreateIndexResource();

	Skeleton CreateSkelton(const Node& rootNode);
	int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);
	SkinCluster CreateSkinCluster(const Microsoft::WRL::ComPtr<ID3D12Device>& device, const Skeleton& skeleton, const ModelData& modelData, const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize);
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInbytes);
	void ApplyAnimation(Skeleton& skeleton, const Animation& animation, float animationTime);

private:
	DirectXCommon* dir_ = DirectXCommon::GetInsTance();
	TextureManager* texture_ = TextureManager::GetInstance();
	Light* light_ = Light::GetInstance();
	CameraForGpu camera;

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
	DirectionalLight directionalLightData;
};