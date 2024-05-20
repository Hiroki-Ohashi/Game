#include "Animation.h"

void AnimationModel::Initialize(const std::string& filename, EulerTransform transform)
{
	model_ = std::make_unique<Model>();
	model_->Initialize(filename, transform);

	animation = texture_->LoadAnimationFile("resources", filename);
	skeleton = CreateSkelton(model_->GetModelData().rootNode);
}

void AnimationModel::Update()
{
	// アニメーションの時間を進める
	animationTime += 0.1f;
	ApplyAnimation(skeleton, animation, animationTime);

	// すべてのJointを更新。親が若いので通常ループで処理可能
	for (Joint& joint : skeleton.joints) {
		joint.localMatrix = MakeAffineMatrixQuaternion(joint.transform.scale, joint.transform.rotate, joint.transform.translate);
		if (joint.parent) { // 親がいれば親の行列をかける
			joint.skeltonSpaceMatrix = Multiply(joint.localMatrix, skeleton.joints[*joint.parent].skeltonSpaceMatrix);
		}
		else { // 親がいないのでLocalMatrixとskeletonSpaceMatrixは一致する
			joint.skeltonSpaceMatrix = joint.localMatrix;
		}
	}
}

void AnimationModel::Draw(Camera* camera, uint32_t index)
{
	model_->DrawAnimation(skeleton, animation, camera, index);
}

Skeleton AnimationModel::CreateSkelton(const Node& rootNode)
{
	Skeleton skeleton;
	skeleton.root = CreateJoint(rootNode, {}, skeleton.joints);

	// 名前とIndexのマッピングを行いアクセスしやすくする
	for (const Joint& joint : skeleton.joints) {
		skeleton.jointmap.emplace(joint.name, joint.index);
	}

	return skeleton;
}

int32_t AnimationModel::CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints)
{
	Joint joint;
	
	joint.name = node.name;
	joint.localMatrix = node.localmatrix;
	joint.skeltonSpaceMatrix = MakeIndentity4x4();
	joint.transform = node.transform;
	joint.index = int32_t(joints.size());
	joint.parent = parent;

	joints.push_back(joint);
	for (const Node& child : node.children) {
		// 子jointを作成し、そのIndexを登録
		int32_t childIndex = CreateJoint(child, joint.index, joints);
		joints[joint.index].children.push_back(childIndex);
	}

	// 自身のIndexを返す
	return joint.index;
}

SkinCluster AnimationModel::CreateSkinCluster(const Microsoft::WRL::ComPtr<ID3D12Device>& device, const Skeleton& skeleton, const ModelData& modelData, const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize)
{
	SkinCluster skinCluster;

	// palette用のResourceを確保
	skinCluster.paletteResource = CreateBufferResource(device, sizeof(WellForGPU) * skeleton.joints.size());
	WellForGPU* mappedPalette = nullptr;
	skinCluster.paletteResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedPalette));
	skinCluster.mappedPalette = { mappedPalette, skeleton.joints.size() }; // spanを使ってアクセスするようにする
	skinCluster.paletteSrvHandle.first = texture_->GetGPUDescriptorHandle(descriptorHeap, descriptorSize, 10);
	// palette用のsrvを作成
	
	// influence用のResourceを確保
	
	// influence用のVBVを作成
	
	// InverseBindPoseMatrixの保存領域を作成
	
	// ModelDataのSkinCluster情報を解析してInfluenceの中身を埋める

	return skinCluster;
}

Microsoft::WRL::ComPtr<ID3D12Resource> AnimationModel::CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInbytes)
{
	Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;

	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	// 頂点リソース用のヒープの設定
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;// UploadHeapを使う

	// 頂点リソースの設定
	D3D12_RESOURCE_DESC ResourceDesc{};
	// バッファリソース。テクスチャの場合はまた別の設定をする
	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	ResourceDesc.Width = sizeInbytes;
	// バッファの場合はこれらは1にする決まり
	ResourceDesc.Height = 1;
	ResourceDesc.DepthOrArraySize = 1;
	ResourceDesc.MipLevels = 1;
	ResourceDesc.SampleDesc.Count = 1;
	// バッファの場合はこれにする決まり
	ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	// 実際に頂点リソースを作る
	HRESULT hr_ = device->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&ResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&Resource));

	assert(SUCCEEDED(hr_));

	return Resource;
}

void AnimationModel::ApplyAnimation(Skeleton& skeleton, const Animation& animation, float animationTime)
{
	for (Joint& joint : skeleton.joints) {
		// 対象のJointのAnimationがあれば、値の適用を行う
		if (auto it = animation.nodeAnimations.find(joint.name); it != animation.nodeAnimations.end()) {
			const NodeAnimation& rootNodeAnimation = (*it).second;
			joint.transform.translate = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime);
			joint.transform.rotate = CalculateValueRotate(rootNodeAnimation.rotate.keyframes, animationTime);
			joint.transform.scale = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime);
		}
	}
}
