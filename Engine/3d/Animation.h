#pragma once
#include "Function.h"
#include "MathFunction.h"
#include "Camera.h"
#include "WorldTransform.h"
#include "TextureManager.h"
#include "Light.h"

/// <summary>
/// Animation.h
/// 3Dアニメーション生成のヘッダーファイル
/// </summary>

class AnimationModel {
public:
	// デストラクタ
	~AnimationModel();

	// 初期化処理
	void Initialize(const std::string& filename, EulerTransform transform, Camera* camera, uint32_t index);
	// 更新処理
	void Update(float time);
	// 描画処理
	void Draw(Camera* camera, uint32_t index, uint32_t index2);

	// Setter
	void SetTranslate(Vector3 translate) const { translate = worldTransform_.translate; }
	void SetScale(Vector3 scale) { scale = worldTransform_.scale; }
	void SetRotate(Vector3 rotate) { rotate = worldTransform_.rotate; }

private:
	// PSO作成
	void CreatePso();
	// vertex生成
	void CreateVertexResource();
	// material作成
	void CreateMaterialResource();
	// wvp作成
	void CreateWVPResource();
	// index作成
	void CreateIndexResource();
	// camera作成
	void CreateCameraResource(Camera* camera);
	// skelton作成
	Skeleton CreateSkelton(const Node& rootNode);
	// joint作成
	int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);
	// skinClister作成
	SkinCluster CreateSkinCluster(const Microsoft::WRL::ComPtr<ID3D12Device>& device, const Skeleton& skeleton, const ModelData& modelData, const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index);
	
	// Resource生成
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInbytes);
	// アニメーション読み込み
	void ApplyAnimation(Skeleton& skeleton, const Animation& animation, float animationTime);

private:
	// シングルトン呼び出し
	DirectXCommon* dir_ = DirectXCommon::GetInsTance();
	TextureManager* texture_ = TextureManager::GetInstance();
	Light* light_ = Light::GetInstance();
	CameraForGpu* camera_ = nullptr;

	// Transform
	WorldTransform worldTransform_;
	EulerTransform uvTransform;

	// 読み込みアニメーション数
	uint32_t index = 0;
	// 読み込み最大数
	static const int maxIndex = 100;

	// animation変数
	Animation animation{};
	// アニメーション再生時間
	float animationTime = 0.0f;

	// skeleton変数
	Skeleton skeleton{};
	// skinCLuster変数
	SkinCluster skinCluster{};
	// joint変数
	Joint joint;

	// PSO
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;

private:
	// map
	uint32_t* mappedIndex;

	// BufferView
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};

	// Resource
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource;

	// Data
	ModelData modelData;
	VertexData* vertexData;
	Material* materialData;
	TransformationMatrix* wvpData;
};