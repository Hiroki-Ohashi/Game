#pragma once
#include "Function.h"
#include<cmath>
#include<cassert>

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