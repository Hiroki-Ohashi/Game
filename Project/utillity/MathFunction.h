#pragma once
#include "Function.h"
#include<cmath>
#include<cassert>
#include <map>
#include <optional>
#include <span>
#include <array>
#include <wrl.h>

/// <summary>
/// MathFuncsion.ｈ
/// 数学関数をまとめたヘッダーファイル
/// </summary>

// Quaternion
struct Quaternion {
	float x;
	float y;
	float z;
	float w;

};

// Vector2
struct Vector2 final {
	float x;
	float y;
};

// Vector3
struct Vector3 final {
	float x;
	float y;
	float z;
};

// Vector4
struct Vector4 final {
	float x;
	float y;
	float z;
	float w;
};

// Matrix4x4
struct Matrix4x4 {
	float m[4][4];
};

// EulerTransform
struct EulerTransform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

// QuaternionTransform
struct QuaternionTransform {
	Vector3 scale;
	Quaternion rotate;
	Vector3 translate;
};

// VertexData
struct VertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

// LevelData
struct LevelData{
	// オブジェクト
	struct ObjectData {
		std::string filename;
		Vector3 translation;
		Vector3 rotation;
		Vector3 scaling;
		// AABBのサイズ
		Vector3 colliderSize;
	};

	// 敵
	struct EnemyData {
		std::string filename;
		Vector3 translation;
		Vector3 rotation;
		Vector3 scaling;
	};

	// 固定の敵
	struct FixedData {
		std::string filename;
		Vector3 translation;
		Vector3 rotation;
		Vector3 scaling;
	};

	std::vector<ObjectData> objects;
	std::vector<EnemyData> enemys;
	std::vector<FixedData> fixedEnemys;
};

// Material
struct Material {
	Vector4 color;
	int32_t enableLighting;
	float padding[3];
	Matrix4x4 uvTransform;
	float shininess;
};

// TransformationMatrix
struct TransformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;
	Matrix4x4 WorldInverseTranspose;
};

// DirectionalLight
struct DirectionalLight {
	Vector4 color;
	Vector3 direction;
	float intensity;
};

// Node
struct Node {
	QuaternionTransform transform;
	Matrix4x4 localmatrix;
	std::string name;
	std::vector<Node> children;
};

// MaterialData
struct MaterialData {
	std::string textureFilePath;
};

// VertexWeightData
struct VertexWeightData {
	float weight;
	uint32_t vertexIndex;
};

// JointWeightData
struct JointWeightData {
	Matrix4x4 inverseBindPoseMatrix;
	std::vector<VertexWeightData> vertexWeights;
};

// ModelData
struct ModelData {
	std::map<std::string, JointWeightData> skinClusterData;
	std::vector<VertexData> vertices;
	std::vector<uint32_t> indices;
	MaterialData material;
	Node rootNode;
};

// Particle
struct Particle {
	EulerTransform transform;
	Vector3 velocity;
	Vector4 color;
	float lifeTime;
	float currentTime;
};

// ParticleForGpu
struct ParticleForGpu {
	Matrix4x4 WVP;
	Matrix4x4 World;
	Vector4 color;
};

// CameraForGpu
struct CameraForGpu {
	Vector3 worldPosition;
};

/// <summary>
/// アニメーション
/// </summary>
/// <typeparam name="tValue"></typeparam>

template <typename tValue>

// Keyframe
struct Keyframe {
	float time; // キーフレームの時刻（単位は秒）
	tValue value; // キーフレームの値
};

using KeyframeVector3 = Keyframe<Vector3>;
using KeyframeQuaternion = Keyframe<Quaternion>;

template <typename tValue>

// AnimationCurve
struct AnimationCurve {
	std::vector<Keyframe<tValue>> keyframes;
};

// NodeAnimation
struct NodeAnimation {
	AnimationCurve<Vector3> translate;
	AnimationCurve<Quaternion> rotate;
	AnimationCurve<Vector3> scale;
};

// Animation
struct Animation {
	float duration; // アニメーション全体の尺（単位は秒）
	// NodeAnimationの集合。Node名でひけるようにしておく
	std::map<std::string, NodeAnimation> nodeAnimations;
};

// Joint
struct Joint {
	QuaternionTransform transform; // Transformの情報
	Matrix4x4 localMatrix; // localMtrix
	Matrix4x4 skeltonSpaceMatrix; // SkeltonSpaceでの変換行列
	std::string name; // 名前
	std::vector<int32_t> children; // 子jointのindexリスト。いらなければ空
	int32_t index; // 自身のindex
	std::optional<int32_t> parent; // 親jointのindex。いなければnull
};

// Skeleton
struct Skeleton {
	int32_t root; // RootJointのindex
	std::map<std::string, int32_t> jointmap; // Joint名とindexの辞書
	std::vector<Joint> joints; // 所属しているJoint
};

const uint32_t kNumMaxInfluence = 4;

// VertexInfluence
struct VertexInfluence {
	std::array<float, kNumMaxInfluence> weights;
	std::array<int32_t, kNumMaxInfluence> jointIndices;
};

// WellForGPU
struct WellForGPU {
	Matrix4x4 skeletonSpaceMatrix; // 位置用
	Matrix4x4 skeletonSpaceInverseTransposeMatrix; // 法線用
};

// SkinCluster
struct SkinCluster {
	std::vector<Matrix4x4> inverseBindPoseMatrices;
	Microsoft::WRL::ComPtr<ID3D12Resource> influenceResource;
	D3D12_VERTEX_BUFFER_VIEW influenceBufferView;
	std::span<VertexInfluence> mappedInfluence;
	Microsoft::WRL::ComPtr<ID3D12Resource> paletteResource;
	std::span<WellForGPU> mappedPalette;
	std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> paletteSrvHandle;
};

// 内積
float Dot(const Vector3& v1, const Vector3& v2);
float Length(const Vector3& v);
// 正規化
Vector3 Normalize(const Vector3& v);
// クロス積
Vector3 Cross(const Vector3& v1, const Vector3& v2);
// 変換
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);
// ベクトル足し算
Vector3 Add(const Vector3& v1, const Vector3& v2);
// ベクトル変換
Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m);
Matrix4x4 Transpose(const Matrix4x4& m);

// 単位行列の作成
Matrix4x4 MakeIndentity4x4();
// 回転行列
Matrix4x4 MakeRotateXMatrix(float radian);
Matrix4x4 MakeRotateYMatrix(float radian);
Matrix4x4 MakeRotateZMatrix(float radian);
// ベクトル積
Matrix4x4 Multiply(const Matrix4x4 m1, const Matrix4x4 m2);
// アフィン変換
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);
// 透視投影行列
Matrix4x4 MakePerspectiveMatrix(float fovY, float aspectRatio, float nearClip, float farClip);
//正射影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);
// ビューポート変換行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);
// 逆行列
Matrix4x4 Inverse(const Matrix4x4& m);
// 正規化
Vector3 Normalize(const Vector3& v1);

Matrix4x4 MakeScaleMatrix(const Vector3& scale);
Matrix4x4 MakeTranslateMatrix(const Vector3& translate);

Matrix4x4 MakeRotateAxisAngle(const Vector3& axis, float angle);
// 線形補間
float LerpShortAngle(float a, float b, float t);
// ベクトル積
Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs);
float Dot(const  Quaternion& q0, const  Quaternion& q1);
// 単位quaternion
Quaternion IdentityQuaternion();
// 共役quaternion
Quaternion Conjugate(const  Quaternion& quaternion);
// quaternionのnorm
float Norm(const  Quaternion& quaternion);
// 正規化
Quaternion Normalize(const  Quaternion& quaternion);
// 逆行列
Quaternion Inverse(const  Quaternion& quaternion);
Quaternion mainasu(const  Quaternion& quaternion);

//任意軸回転
Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle);
// ベクトルをquaternion回転
Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion);
// Quaternionから回転行列を求める
Matrix4x4 MakeRotateMatrix(const Quaternion quaternion);
// 線形補間
Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);
Quaternion LerpQuaternion(const Quaternion& v1, const Quaternion& v2, float t);
// 球面補間
Vector3 Slerp(const Vector3& v1, const Vector3& v2, float t);
Quaternion SlerpQuaternion(const Quaternion& q0, const Quaternion& q1, float t);

Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time);
Quaternion CalculateValueRotate(const std::vector<KeyframeQuaternion>& keyframes, float time);
Matrix4x4 MakeAffineMatrixQuaternion(const Vector3& scale, const Quaternion& rotate, const Vector3& translate);
