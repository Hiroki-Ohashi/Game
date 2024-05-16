#pragma once
#include "Function.h"
#include<cmath>
#include<cassert>
#include <map>
#include <optional>

struct Quaternion {
	float x;
	float y;
	float z;
	float w;

};

struct Vector2 final {
	float x;
	float y;
};

struct Vector3 final {
	float x;
	float y;
	float z;
};

struct Vector4 final {
	float x;
	float y;
	float z;
	float w;
};

struct Matrix4x4 {
	float m[4][4];
};

struct EulerTransform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

struct QuaternionTransform {
	Vector3 scale;
	Quaternion rotate;
	Vector3 translate;
};

struct VertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

struct Material {
	Vector4 color;
	int32_t enableLighting;
	float padding[3];
	Matrix4x4 uvTransform;
	float shininess;
};

struct TransformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;
};

struct DirectionalLight {
	Vector4 color;
	Vector3 direction;
	float intensity;
};

struct Node {
	QuaternionTransform transform;
	Matrix4x4 localmatrix;
	std::string name;
	std::vector<Node> children;
};

struct MaterialData {
	std::string textureFilePath;
};

struct ModelData {
	std::vector<VertexData> vertices;
	std::vector<uint32_t> indices;
	MaterialData material;
	Node rootNode;
};

struct Particle {
	EulerTransform transform;
	Vector3 velocity;
	Vector4 color;
	float lifeTime;
	float currentTime;
};

struct ParticleForGpu {
	Matrix4x4 WVP;
	Matrix4x4 World;
	Vector4 color;
};

struct CameraForGpu {
	Vector3 worldPosition;
};

template <typename tValue>
struct Keyframe {
	float time; // キーフレームの時刻（単位は秒）
	tValue value; // キーフレームの値
};

using KeyframeVector3 = Keyframe<Vector3>;
using KeyframeQuaternion = Keyframe<Quaternion>;

template <typename tValue>

struct AnimationCurve {
	std::vector<Keyframe<tValue>> keyframes;
};

struct NodeAnimation {
	AnimationCurve<Vector3> translate;
	AnimationCurve<Quaternion> rotate;
	AnimationCurve<Vector3> scale;
};

struct Animation {
	float duration; // アニメーション全体の尺（単位は秒）
	// NodeAnimationの集合。Node名でひけるようにしておく
	std::map<std::string, NodeAnimation> nodeAnimations;
};

struct Joint {
	QuaternionTransform transform; // Transformの情報
	Matrix4x4 localMatrix; // localMtrix
	Matrix4x4 skeltonSpaceMatrix; // SkeltonSpaceでの変換行列
	std::string name; // 名前
	std::vector<int32_t> children; // 子jointのindexリスト。いらなければ空
	int32_t index; // 自身のindex
	std::optional<int32_t> parent; // 親jointのindex。いなければnull
};

struct Skeleton {
	int32_t root; // RootJointのindex
	std::map<std::string, int32_t> jointmap; // Joint名とindexの辞書
	std::vector<Joint> joints; // 所属しているJoint
};

float Dot(const Vector3& v1, const Vector3& v2);
float Length(const Vector3& v);
Vector3 Normalize(const Vector3& v1);
Vector3 Cross(const Vector3& v1, const Vector3& v2);
Vector3 Transforme(const Vector3& vector, const Matrix4x4& matrix);

// 単位行列の作成
Matrix4x4 MakeIndentity4x4();

Matrix4x4 MakeRotateXMatrix(float radian);
Matrix4x4 MakeRotateYMatrix(float radian);
Matrix4x4 MakeRotateZMatrix(float radian);

Matrix4x4 Multiply(const Matrix4x4 m1, const Matrix4x4 m2);

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);
Matrix4x4 MakePerspectiveMatrix(float fovY, float aspectRatio, float nearClip, float farClip);
Matrix4x4 MakeOrthographicMatrix(float left, float right, float top, float bottom, float nearClip, float farClip);

Matrix4x4 Inverse(const Matrix4x4& m);

Vector3 Normalize(const Vector3& v1);

Matrix4x4 MakeScaleMatrix(const Vector3& scale);
Matrix4x4 MakeTranslateMatrix(const Vector3& translate);

Matrix4x4 MakeRotateAxisAngle(const Vector3& axis, float angle);

Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs);
float Dot(const  Quaternion& q0, const  Quaternion& q1);
Quaternion IdentityQuaternion();
Quaternion Conjugate(const  Quaternion& quaternion);
float Norm(const  Quaternion& quaternion);
Quaternion Normalize(const  Quaternion& quaternion);
Quaternion Inverse(const  Quaternion& quaternion);

Quaternion mainasu(const  Quaternion& quaternion);

//任意軸回転
Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle);
// ベクトルをquaternion回転
Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion);
// Quaternionから回転行列を求める
Matrix4x4 MakeRotateMatrix(const Quaternion quaternion);

Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);
Quaternion LerpQuaternion(const Quaternion& v1, const Quaternion& v2, float t);

Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t);

Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time);
Quaternion CalculateValueRotate(const std::vector<KeyframeQuaternion>& keyframes, float time);
Matrix4x4 MakeAffineMatrixQuaternion(const Vector3& scale, const Quaternion& rotate, const Vector3& translate);
