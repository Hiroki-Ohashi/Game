#include "TextureManager.h"

/// <summary>
/// Texture.cpp
/// テクスチャ生成のソースファイル
/// </summary>
namespace Engine
{
	TextureManager* TextureManager::GetInstance()
	{
		static TextureManager instance;
		return &instance;
	}

	void TextureManager::Initialize()
	{
		descriptorSizeDSV = dir_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		descriptorSizeRTV = dir_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		descriptorSizeSRV = dir_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		textureIndex_ = -1;
	}

	uint32_t TextureManager::Load(const std::string& filePath)
	{
		textureIndex_ += 1;
		SetTexture(filePath, textureIndex_);
		return textureIndex_;
	}

	Animation TextureManager::LoadAnimation(const std::string& directoryPath, const std::string& filename)
	{
		animationIndex_ += 1;
		animation[animationIndex_] = LoadAnimationFile(directoryPath, filename);
		return animation[animationIndex_];
	}

	ModelData TextureManager::LoadModelFile(const std::string& directoryPath, const std::string& filename)
	{
		ModelData modelData; // 構築するModelData

		Assimp::Importer importer;
		std::string filePath = directoryPath + "/" + filename;
		const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
		assert(scene->HasMeshes()); // メッシュがないのは対応しない

		for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
			aiMesh* mesh = scene->mMeshes[meshIndex];
			assert(mesh->HasNormals()); // 法線がないMeshは今回非対応
			assert(mesh->HasTextureCoords(0)); // TexcoordがないMeshは今回非対応
			modelData.vertices.resize(mesh->mNumVertices); // 最初に頂点数分のメモリを確保しておく

			for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex) {
				aiVector3D& position = mesh->mVertices[vertexIndex];
				aiVector3D& normal = mesh->mNormals[vertexIndex];
				aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
				// 右手系->左手系への変換を忘れずに
				modelData.vertices[vertexIndex].position = { -position.x, position.y, position.z, 1.0f };
				modelData.vertices[vertexIndex].normal = { -normal.x, normal.y, normal.z };
				modelData.vertices[vertexIndex].texcoord = { texcoord.x, texcoord.y };
			}
			// Index解析
			for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
				aiFace& face = mesh->mFaces[faceIndex];
				assert(face.mNumIndices == 3);

				for (uint32_t element = 0; element < face.mNumIndices; ++element) {
					uint32_t vertexIndex = face.mIndices[element];
					modelData.indices.push_back(vertexIndex);
				}
			}

			// SkinCluster
			for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
				// Jointごとの格納領域を作る
				aiBone* bone = mesh->mBones[boneIndex];
				std::string jointName = bone->mName.C_Str();
				JointWeightData& jointWeightData = modelData.skinClusterData[jointName];

				// InverseBindPoseMatrixの抽出
				aiMatrix4x4 bindPoseMatrixAssimp = bone->mOffsetMatrix.Inverse();// BindPoseMatrixに戻す
				aiVector3D scale, translate;
				aiQuaternion rotate;
				bindPoseMatrixAssimp.Decompose(scale, rotate, translate);// 成分を抽出
				// 左手座標系のBindPoseMatrixを作る
				Matrix4x4 bindPoseMatrix = MakeAffineMatrixQuaternion(
					{ scale.x,scale.y,scale.z }, { rotate.x,-rotate.y,-rotate.z,rotate.w }, { -translate.x,translate.y,translate.z });
				// InverseBindPoseMatrixにする
				jointWeightData.inverseBindPoseMatrix = Inverse(bindPoseMatrix);

				// Weight情報を取り出す
				for (uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex) {
					jointWeightData.vertexWeights.push_back({ bone->mWeights[weightIndex].mWeight, bone->mWeights[weightIndex].mVertexId });
				}
			}

			for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
				aiMaterial* material = scene->mMaterials[materialIndex];
				if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
					aiString textureFilepath;
					material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilepath);
					modelData.material.textureFilePath = directoryPath + "/" + textureFilepath.C_Str();
				}
			}

			modelData.rootNode = ReadNode(scene->mRootNode);
		}
		return modelData;
	}

	MaterialData TextureManager::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
		// 必要な変数の宣言
		MaterialData materialData; // 構築するModelData
		std::string line; // ファイルから読んだ1行を格納するもの
		std::ifstream file(directoryPath + "/" + filename); // ファイルを開く
		assert(file.is_open()); // とりあえず開けなかったら止める

		while (std::getline(file, line)) {
			std::string identifier;
			std::istringstream s(line);
			s >> identifier; // 先頭の識別子を読む

			// identifierに応じた処理
			if (identifier == "map_Kd") {
				std::string textureFilename;
				s >> textureFilename;
				// 連結してファイルパスにする
				materialData.textureFilePath = directoryPath + "/" + textureFilename;
			}
		}
		return materialData;
	}

	ModelData TextureManager::LoadObjFile(const std::string& directoryPath, const std::string& filename)
	{
		// キャッシュ確認
		{
			std::lock_guard<std::mutex> lock(cacheMutex);
			auto it = modelCache.find(directoryPath + "/" + filename);
			if (it != modelCache.end()) {
				return it->second; // キャッシュに存在すればそれを返す
			}
		}

		// キャッシュに存在しない場合は新たにロード

		// 必要な変数の宣言
		ModelData modelData; // 構築するModelData
		std::vector<Vector4> positions; // 位置
		std::vector<Vector3> normals; // 法線
		std::vector<Vector2> texcoords; // テクスチャ座標
		std::string line; // ファイルから読んだ1行を格納するもの

		// ファイルを開くために開く
		std::ifstream file(directoryPath + "/" + filename); // ファイルを開く
		assert(file.is_open()); // とりあえず開けなかったら止める

		while (std::getline(file, line)) {
			std::string identifier;
			std::istringstream s(line);
			s >> identifier; // 先頭の識別子を読む
			if (identifier == "v") {
				Vector4 position;
				s >> position.x >> position.y >> position.z;
				position.z *= -1.0f;
				position.w = 1.0f;
				positions.push_back(position);
			}
			else if (identifier == "vt") {
				Vector2 texcoord;
				s >> texcoord.x >> texcoord.y;
				texcoord.y = 1.0f - texcoord.y;
				texcoords.push_back(texcoord);
			}
			else if (identifier == "vn") {
				Vector3 normal;
				s >> normal.x >> normal.y >> normal.z;
				normal.z *= -1.0f;
				normals.push_back(normal);
			}
			else if (identifier == "f") {
				VertexData triangle[3];
				// 面は三角形限定。その他は未対応
				for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
					std::string  vertexDefinition;
					s >> vertexDefinition;
					// 頂点の要素へのIndexは「位置/uv/法線」で格納されているので、分解してIndexを取得する
					std::istringstream v(vertexDefinition);
					uint32_t  elementIndices[3];
					for (int32_t element = 0; element < 3; ++element) {
						std::string index;
						std::getline(v, index, '/'); // 区切りでインデックスを読んでいく
						elementIndices[element] = std::stoi(index);
					}
					// 要素へのIndexから、実際の要素の値を取得して、頂点を構築する
					Vector4 position = positions[elementIndices[0] - 1];
					Vector2 texcoord = texcoords[elementIndices[1] - 1];
					Vector3 normal = normals[elementIndices[2] - 1];
					VertexData vertex = { position, texcoord, normal };
					modelData.vertices.push_back(vertex);
					triangle[faceVertex] = { position, texcoord, normal };
				}
				// 頂点を逆順で登録することで、回り順を逆にする
				modelData.vertices.push_back(triangle[2]);
				modelData.vertices.push_back(triangle[1]);
				modelData.vertices.push_back(triangle[0]);
			}
			else if (identifier == "mtllib") {
				// materialtemplateLibraryファイルの名前を取得する
				std::string materialFilename;
				s >> materialFilename;
				// 基本的にobjファイルと同一階層にmtlは存在させるので、ディレクトリ名とファイル名を渡す
				modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
			}

		}

		// キャッシュに保存
		{
			std::lock_guard<std::mutex> lock(cacheMutex);
			modelCache[directoryPath + "/" + filename] = modelData;
		}
		return modelData;
	}

	Animation TextureManager::LoadAnimationFile(const std::string& directoryPath, const std::string& filename)
	{
		Animation animation_; //今回作るアニメーション

		Assimp::Importer importor;
		std::string filePath = directoryPath + "/" + filename;
		const aiScene* scene = importor.ReadFile(filePath.c_str(), 0);
		assert(scene->mNumAnimations != 0); // アニメーションがない
		aiAnimation* animationAssimp = scene->mAnimations[0]; // 最初のアニメーションだけ採用。もちろん複数対応するにこしたことはない
		animation_.duration = float(animationAssimp->mDuration / animationAssimp->mTicksPerSecond); // 時間の単位を秒に変換

		// assimpでは個々のNodeのAnimationをchannelと呼んでいるのでchannelを回してNodeAnimationの情報をとってくる
		for (uint32_t channelIndex = 0; channelIndex < animationAssimp->mNumChannels; ++channelIndex) {

			aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
			NodeAnimation& nodeAnimation = animation_.nodeAnimations[nodeAnimationAssimp->mNodeName.C_Str()];

			// translate
			for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumPositionKeys; ++keyIndex) {
				aiVectorKey& keyAssimp = nodeAnimationAssimp->mPositionKeys[keyIndex];
				KeyframeVector3 keyframe;
				keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);// ここも秒に変換
				keyframe.value = { -keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z }; // 右手->左手
				nodeAnimation.translate.keyframes.push_back(keyframe);
			}

			// Rotate
			for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumRotationKeys; ++keyIndex) {
				aiQuatKey& keyAssimp = nodeAnimationAssimp->mRotationKeys[keyIndex];
				KeyframeQuaternion keyframe;
				keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);// ここも秒に変換
				keyframe.value = { keyAssimp.mValue.x, -keyAssimp.mValue.y, -keyAssimp.mValue.z, keyAssimp.mValue.w }; // 右手->左手
				nodeAnimation.rotate.keyframes.push_back(keyframe);
			}

			// scale
			for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumScalingKeys; ++keyIndex) {
				aiVectorKey& keyAssimp = nodeAnimationAssimp->mScalingKeys[keyIndex];
				KeyframeVector3 keyframe;
				keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);// ここも秒に変換
				keyframe.value = { keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z }; // 右手->左手
				nodeAnimation.scale.keyframes.push_back(keyframe);
			}

		}

		return animation_;
	}

	Node TextureManager::ReadNode(aiNode* node)
	{
		Node result;

		aiVector3D scale, translate;
		aiQuaternion rotate;
		node->mTransformation.Decompose(scale, rotate, translate);// assimpの行列からSRTを抽出する関数を利用

		result.transform.scale = { scale.x,scale.y,scale.z }; // Scaleはそのまま
		result.transform.rotate = { rotate.x, -rotate.y, -rotate.z, rotate.w }; // x軸を反転、さらに回転方向が逆なので軸を反転させる
		result.transform.translate = { -translate.x, translate.y, translate.z }; // x軸を反転
		result.localmatrix = MakeAffineMatrixQuaternion(result.transform.scale, result.transform.rotate, result.transform.translate);

		result.name = node->mName.C_Str();// node名を確定
		result.children.resize(node->mNumChildren);// 子供の数だけ確保
		for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex) {
			// 再帰的に読んで階層構造を作っていく
			result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
		}

		return result;
	}

	void TextureManager::SetTexture(const std::string& filePath, uint32_t index)
	{
		// キャッシュを確認
		{
			std::lock_guard<std::mutex> lock(TextureCacheMutex);
			auto it = textureCache.find(filePath);
			if (it != textureCache.end()) {
				// キャッシュに存在する場合
				textureResource[index] = it->second;
				textureSrvHandleCPU[index] = GetCPUDescriptorHandle(dir_->GetSrvDescriptorHeap2(), descriptorSizeSRV, index);
				textureSrvHandleGPU[index] = GetGPUDescriptorHandle(dir_->GetSrvDescriptorHeap2(), descriptorSizeSRV, index);

				// SRVをキャッシュから取得して設定
				dir_->GetDevice()->CreateShaderResourceView(
					textureResource[index].Get(),
					&srvCache[filePath],
					textureSrvHandleCPU[index]
				);
				return;
			}
		}

		// キャッシュに存在しない場合は新たにロード
		// Textureを読んで転送する
		DirectX::ScratchImage mipImages = LoadTexture(filePath);
		const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
		textureResource[index] = CreateTextureResource(dir_->GetDevice(), metadata);
		intermediateResource[index] = UploadTextureData(textureResource[index].Get(), mipImages);

		// metaDataを基にSRVの設定
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = metadata.format;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		if (metadata.IsCubemap()) {
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
			srvDesc.TextureCube.MostDetailedMip = 0; // unionがTextureCubeになったが、内部パラメータの意味はTexture2dと変わらない
			srvDesc.TextureCube.MipLevels = UINT_MAX;
			srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
		}
		else {
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;// 2Dテクスチャ
			srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);
		}

		// SRVを作成するDescriptorHeapの場所を決める
		textureSrvHandleCPU[index] = GetCPUDescriptorHandle(dir_->GetSrvDescriptorHeap2(), descriptorSizeSRV, index);
		textureSrvHandleGPU[index] = GetGPUDescriptorHandle(dir_->GetSrvDescriptorHeap2(), descriptorSizeSRV, index);

		// 先頭はImGuiが使っているのでその次を使う
		textureSrvHandleCPU[index].ptr += dir_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		textureSrvHandleGPU[index].ptr += dir_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		// SRVの生成
		dir_->GetDevice()->CreateShaderResourceView(textureResource[index].Get(), &srvDesc, textureSrvHandleCPU[index]);

		// キャッシュに保存
		{
			std::lock_guard<std::mutex> lock(TextureCacheMutex);
			textureCache[filePath] = textureResource[index];
			srvCache[filePath] = srvDesc;
		}
	}

	Microsoft::WRL::ComPtr<ID3D12Resource> TextureManager::CreateTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, const DirectX::TexMetadata& metadata)
	{
		// metadataを基にResourceの設定
		D3D12_RESOURCE_DESC resourceDesc{};
		resourceDesc.Width = UINT(metadata.width); // textureの幅
		resourceDesc.Height = UINT(metadata.height); // textureの高さ
		resourceDesc.MipLevels = UINT16(metadata.mipLevels); // mipmapの数
		resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize); // 奥行 or 配列textureの配列数
		resourceDesc.Format = metadata.format; // textureのformat
		resourceDesc.SampleDesc.Count = 1; // サンプリングカウント。1固定。
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension); // textureの次元数。普段使っているのは2次元

		// 利用するHeapの設定。非常に特殊な運用。02_04exで一般的なケース番がある
		D3D12_HEAP_PROPERTIES heapProperties{};
		heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		/*heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
		heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;*/

		// Resourceの生成
		Microsoft::WRL::ComPtr< ID3D12Resource> resource = nullptr;

		[[maybe_unused]] HRESULT hr = device->CreateCommittedResource(
			&heapProperties, // Heapの設定
			D3D12_HEAP_FLAG_NONE, // Heapの特殊な設定。特になし
			&resourceDesc, // Resourceの設定
			D3D12_RESOURCE_STATE_COPY_DEST, // 初回のResourceState。Textureは基本読むだけ
			nullptr, // Clear最適値。使わないのでnullptr
			IID_PPV_ARGS(&resource)); // 作成するResourceポインタへのポインタ

		assert(SUCCEEDED(hr));

		return resource;
	}

	DirectX::ScratchImage TextureManager::LoadTexture(const std::string& filePath) {

		// テクスチャファイルを読んでプログラムで扱えるようにする
		DirectX::ScratchImage image{};
		std::wstring filePathW = Convert::ConvertString(filePath);;
		HRESULT hr;
		if (filePathW.ends_with(L".dds")) {// .ddsで終わっていたらddsとみなす。
			hr = DirectX::LoadFromDDSFile(filePathW.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
		}
		else {
			hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
		}

		//assert(SUCCEEDED(hr));

		// ミップマップの作成
		DirectX::ScratchImage mipImages{};
		if (DirectX::IsCompressed(image.GetMetadata().format)) {// 圧縮フォーマットかどうか調べる
			mipImages = std::move(image); // 圧縮フォーマットならそのまま使うのでmoveする
		}
		else {
			hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 4, mipImages);
		}

		//assert(SUCCEEDED(hr));

		return mipImages;
	}

	[[nodiscard]]
	Microsoft::WRL::ComPtr<ID3D12Resource> TextureManager::UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages)
	{
		std::vector<D3D12_SUBRESOURCE_DATA>subresources;
		DirectX::PrepareUpload(dir_->GetDevice().Get(), mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
		uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresources.size()));
		Microsoft::WRL::ComPtr< ID3D12Resource> intermediateResource_ = CreateBufferResource(dir_->GetDevice(), intermediateSize);
		UpdateSubresources(dir_->GetCommandList().Get(), texture, intermediateResource_.Get(), 0, 0, UINT(subresources.size()), subresources.data());
		//Tetureへの転送後は利用できるようにD3D12_RESOURCE_STATE_COPY_DESTからD3D12_RESOURCE_STATE_GENERIC_READへResourceStateを変更する
		D3D12_RESOURCE_BARRIER barrier{};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = texture;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
		dir_->GetCommandList()->ResourceBarrier(1, &barrier);
		return intermediateResource_;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE TextureManager::GetCPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index) {
		D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
		handleCPU.ptr += (descriptorSize * index);
		return handleCPU;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetGPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index) {
		D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
		handleGPU.ptr += (descriptorSize * index);
		return handleGPU;
	}

	Microsoft::WRL::ComPtr<ID3D12Resource> TextureManager::CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInbytes)
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
		[[maybe_unused]] HRESULT hr = device->CreateCommittedResource(
			&uploadHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&ResourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&Resource));

		assert(SUCCEEDED(hr));

		return Resource;
	}
}