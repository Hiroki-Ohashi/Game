#pragma once
#include "Function.h"
#include<cmath>
#include<cassert>

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

struct Transform {
	Vector3 scale;
	Vector3 rotate;
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
	Matrix4x4 localmatrix;
	std::string name;
	std::vector<Node> children;
};

struct MaterialData {
	std::string textureFilePath;
};

struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
	Node rootNode;
};

struct Particle {
	Transform transform;
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

Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t);