#include "MathFunction.h"

float Dot(const Vector3& v1, const Vector3& v2)
{
	float result;
	result = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	return result;
}

float Length(const Vector3& v)
{
	float result;
	result = powf(v.x, 2.0) + powf(v.y, 2.0) + powf(v.z, 2.0);

	return sqrtf(result);
};

Vector3 Cross(const Vector3& v1, const Vector3& v2)
{
	Vector3 Cross;
	Cross.x = v1.y * v2.z - v1.z * v2.y;
	Cross.y = v1.z * v2.x - v1.x * v2.z;
	Cross.z = v1.x * v2.y - v1.y * v2.x;
	return Cross;
}

Vector3 Transforme(const Vector3& vector, const Matrix4x4& matrix) {
	Vector3 result;
	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];
	assert(w != 0.0f);
	result.x /= w;
	result.y /= w;
	result.z /= w;

	return result;
}

Matrix4x4 MakeIndentity4x4()
{
	Matrix4x4 MakeIndentity4x4;

	MakeIndentity4x4.m[0][0] = 1;
	MakeIndentity4x4.m[0][1] = 0;
	MakeIndentity4x4.m[0][2] = 0;
	MakeIndentity4x4.m[0][3] = 0;

	MakeIndentity4x4.m[1][0] = 0;
	MakeIndentity4x4.m[1][1] = 1;
	MakeIndentity4x4.m[1][2] = 0;
	MakeIndentity4x4.m[1][3] = 0;

	MakeIndentity4x4.m[2][0] = 0;
	MakeIndentity4x4.m[2][1] = 0;
	MakeIndentity4x4.m[2][2] = 1;
	MakeIndentity4x4.m[2][3] = 0;

	MakeIndentity4x4.m[3][0] = 0;
	MakeIndentity4x4.m[3][1] = 0;
	MakeIndentity4x4.m[3][2] = 0;
	MakeIndentity4x4.m[3][3] = 1;

	return MakeIndentity4x4;
}

Matrix4x4 MakeRotateXMatrix(float radian) {
	Matrix4x4 MakeRotateXMatrix;
	MakeRotateXMatrix.m[0][0] = 1;
	MakeRotateXMatrix.m[0][1] = 0;
	MakeRotateXMatrix.m[0][2] = 0;
	MakeRotateXMatrix.m[0][3] = 0;

	MakeRotateXMatrix.m[1][0] = 0;
	MakeRotateXMatrix.m[1][1] = std::cos(radian);
	MakeRotateXMatrix.m[1][2] = std::sin(radian);
	MakeRotateXMatrix.m[1][3] = 0;

	MakeRotateXMatrix.m[2][0] = 0;
	MakeRotateXMatrix.m[2][1] = -std::sin(radian);
	MakeRotateXMatrix.m[2][2] = std::cos(radian);
	MakeRotateXMatrix.m[2][3] = 0;

	MakeRotateXMatrix.m[3][0] = 0;
	MakeRotateXMatrix.m[3][1] = 0;
	MakeRotateXMatrix.m[3][2] = 0;
	MakeRotateXMatrix.m[3][3] = 1;

	return MakeRotateXMatrix;
}

Matrix4x4 MakeRotateYMatrix(float radian) {
	Matrix4x4 MakeRotateYMatrix;
	MakeRotateYMatrix.m[0][0] = std::cos(radian);
	MakeRotateYMatrix.m[0][1] = 0;
	MakeRotateYMatrix.m[0][2] = -std::sin(radian);
	MakeRotateYMatrix.m[0][3] = 0;

	MakeRotateYMatrix.m[1][0] = 0;
	MakeRotateYMatrix.m[1][1] = 1;
	MakeRotateYMatrix.m[1][2] = 0;
	MakeRotateYMatrix.m[1][3] = 0;

	MakeRotateYMatrix.m[2][0] = std::sin(radian);
	MakeRotateYMatrix.m[2][1] = 0;
	MakeRotateYMatrix.m[2][2] = std::cos(radian);
	MakeRotateYMatrix.m[2][3] = 0;

	MakeRotateYMatrix.m[3][0] = 0;
	MakeRotateYMatrix.m[3][1] = 0;
	MakeRotateYMatrix.m[3][2] = 0;
	MakeRotateYMatrix.m[3][3] = 1;

	return MakeRotateYMatrix;
}

Matrix4x4 MakeRotateZMatrix(float radian) {
	Matrix4x4 MakeRotateZMatrix;
	MakeRotateZMatrix.m[0][0] = std::cos(radian);
	MakeRotateZMatrix.m[0][1] = std::sin(radian);
	MakeRotateZMatrix.m[0][2] = 0;
	MakeRotateZMatrix.m[0][3] = 0;

	MakeRotateZMatrix.m[1][0] = -std::sin(radian);
	MakeRotateZMatrix.m[1][1] = std::cos(radian);
	MakeRotateZMatrix.m[1][2] = 0;
	MakeRotateZMatrix.m[1][3] = 0;

	MakeRotateZMatrix.m[2][0] = 0;
	MakeRotateZMatrix.m[2][1] = 0;
	MakeRotateZMatrix.m[2][2] = 1;
	MakeRotateZMatrix.m[2][3] = 0;

	MakeRotateZMatrix.m[3][0] = 0;
	MakeRotateZMatrix.m[3][1] = 0;
	MakeRotateZMatrix.m[3][2] = 0;
	MakeRotateZMatrix.m[3][3] = 1;

	return MakeRotateZMatrix;
}

Matrix4x4 Multiply(const Matrix4x4 m1, const Matrix4x4 m2) {
	Matrix4x4 m4;
	m4.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[0][1] * m2.m[1][0] + m1.m[0][2] * m2.m[2][0] + m1.m[0][3] * m2.m[3][0];
	m4.m[0][1] = m1.m[0][0] * m2.m[0][1] + m1.m[0][1] * m2.m[1][1] + m1.m[0][2] * m2.m[2][1] + m1.m[0][3] * m2.m[3][1];
	m4.m[0][2] = m1.m[0][0] * m2.m[0][2] + m1.m[0][1] * m2.m[1][2] + m1.m[0][2] * m2.m[2][2] + m1.m[0][3] * m2.m[3][2];
	m4.m[0][3] = m1.m[0][0] * m2.m[0][3] + m1.m[0][1] * m2.m[1][3] + m1.m[0][2] * m2.m[2][3] + m1.m[0][3] * m2.m[3][3];

	m4.m[1][0] = m1.m[1][0] * m2.m[0][0] + m1.m[1][1] * m2.m[1][0] + m1.m[1][2] * m2.m[2][0] + m1.m[1][3] * m2.m[3][0];
	m4.m[1][1] = m1.m[1][0] * m2.m[0][1] + m1.m[1][1] * m2.m[1][1] + m1.m[1][2] * m2.m[2][1] + m1.m[1][3] * m2.m[3][1];
	m4.m[1][2] = m1.m[1][0] * m2.m[0][2] + m1.m[1][1] * m2.m[1][2] + m1.m[1][2] * m2.m[2][2] + m1.m[1][3] * m2.m[3][2];
	m4.m[1][3] = m1.m[1][0] * m2.m[0][3] + m1.m[1][1] * m2.m[1][3] + m1.m[1][2] * m2.m[2][3] + m1.m[1][3] * m2.m[3][3];

	m4.m[2][0] = m1.m[2][0] * m2.m[0][0] + m1.m[2][1] * m2.m[1][0] + m1.m[2][2] * m2.m[2][0] + m1.m[2][3] * m2.m[3][0];
	m4.m[2][1] = m1.m[2][0] * m2.m[0][1] + m1.m[2][1] * m2.m[1][1] + m1.m[2][2] * m2.m[2][1] + m1.m[2][3] * m2.m[3][1];
	m4.m[2][2] = m1.m[2][0] * m2.m[0][2] + m1.m[2][1] * m2.m[1][2] + m1.m[2][2] * m2.m[2][2] + m1.m[2][3] * m2.m[3][2];
	m4.m[2][3] = m1.m[2][0] * m2.m[0][3] + m1.m[2][1] * m2.m[1][3] + m1.m[2][2] * m2.m[2][3] + m1.m[2][3] * m2.m[3][3];

	m4.m[3][0] = m1.m[3][0] * m2.m[0][0] + m1.m[3][1] * m2.m[1][0] + m1.m[3][2] * m2.m[2][0] + m1.m[3][3] * m2.m[3][0];
	m4.m[3][1] = m1.m[3][0] * m2.m[0][1] + m1.m[3][1] * m2.m[1][1] + m1.m[3][2] * m2.m[2][1] + m1.m[3][3] * m2.m[3][1];
	m4.m[3][2] = m1.m[3][0] * m2.m[0][2] + m1.m[3][1] * m2.m[1][2] + m1.m[3][2] * m2.m[2][2] + m1.m[3][3] * m2.m[3][2];
	m4.m[3][3] = m1.m[3][0] * m2.m[0][3] + m1.m[3][1] * m2.m[1][3] + m1.m[3][2] * m2.m[2][3] + m1.m[3][3] * m2.m[3][3];
	return m4;
}


Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	Matrix4x4 MakeAffineMatrix;


	MakeRotateXMatrix(rotate.x);
	MakeRotateYMatrix(rotate.y);
	MakeRotateZMatrix(rotate.z);

	Matrix4x4 XYZ = Multiply(MakeRotateXMatrix(rotate.x), Multiply(MakeRotateYMatrix(rotate.y), MakeRotateZMatrix(rotate.z)));

	MakeAffineMatrix.m[0][0] = XYZ.m[0][0] * scale.x;
	MakeAffineMatrix.m[0][1] = XYZ.m[0][1] * scale.x;
	MakeAffineMatrix.m[0][2] = XYZ.m[0][2] * scale.x;
	MakeAffineMatrix.m[0][3] = 0;

	MakeAffineMatrix.m[1][0] = XYZ.m[1][0] * scale.y;
	MakeAffineMatrix.m[1][1] = XYZ.m[1][1] * scale.y;
	MakeAffineMatrix.m[1][2] = XYZ.m[1][2] * scale.y;
	MakeAffineMatrix.m[1][3] = 0;

	MakeAffineMatrix.m[2][0] = XYZ.m[2][0] * scale.z;
	MakeAffineMatrix.m[2][1] = XYZ.m[2][1] * scale.z;
	MakeAffineMatrix.m[2][2] = XYZ.m[2][2] * scale.z;
	MakeAffineMatrix.m[2][3] = 0;

	MakeAffineMatrix.m[3][0] = translate.x;
	MakeAffineMatrix.m[3][1] = translate.y;
	MakeAffineMatrix.m[3][2] = translate.z;
	MakeAffineMatrix.m[3][3] = 1;

	return MakeAffineMatrix;
}

Matrix4x4 MakePerspectiveMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	Matrix4x4 mpm;
	mpm.m[0][0] = 1 / aspectRatio * (1 / std::tan(fovY / 2));
	mpm.m[0][1] = 0;
	mpm.m[0][2] = 0;
	mpm.m[0][3] = 0;

	mpm.m[1][0] = 0;
	mpm.m[1][1] = 1 / std::tan(fovY / 2);
	mpm.m[1][2] = 0;
	mpm.m[1][3] = 0;

	mpm.m[2][0] = 0;
	mpm.m[2][1] = 0;
	mpm.m[2][2] = farClip / (farClip - nearClip);
	mpm.m[2][3] = 1;

	mpm.m[3][0] = 0;
	mpm.m[3][1] = 0;
	mpm.m[3][2] = (-nearClip * farClip) / (farClip - nearClip);
	mpm.m[3][3] = 0;
	return mpm;
}

Matrix4x4 Inverse(const Matrix4x4& m) {
	float m4s;
	Matrix4x4 m4;
	m4s =
		m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2]
		- m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1] - m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2]
		- m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1] - m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2]
		+ m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1] + m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2]
		+ m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2]
		- m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1] - m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2]
		- m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0] - m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0] - m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0]
		+ m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0] + m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0];
	m4.m[0][0] = 1 / m4s * (
		m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[1][3] * m.m[2][1] * m.m[3][2]
		- m.m[1][3] * m.m[2][2] * m.m[3][1] - m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[1][1] * m.m[2][3] * m.m[3][2]
		);
	m4.m[0][1] = 1 / m4s * (
		-m.m[0][1] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[2][3] * m.m[3][1] - m.m[0][3] * m.m[2][1] * m.m[3][2]
		+ m.m[0][3] * m.m[2][2] * m.m[3][1] + m.m[0][2] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[2][3] * m.m[3][2]);
	m4.m[0][2] = 1 / m4s * (
		m.m[0][1] * m.m[1][2] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[3][2]
		- m.m[0][3] * m.m[1][2] * m.m[3][1] - m.m[0][2] * m.m[1][1] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[3][2]);
	m4.m[0][3] = 1 / m4s * (
		-m.m[0][1] * m.m[1][2] * m.m[2][3] - m.m[0][2] * m.m[1][3] * m.m[2][1] - m.m[0][3] * m.m[1][1] * m.m[2][2]
		+ m.m[0][3] * m.m[1][2] * m.m[2][1] + m.m[0][2] * m.m[1][1] * m.m[2][3] + m.m[0][1] * m.m[1][3] * m.m[2][2]);

	//2
	m4.m[1][0] = 1 / m4s * (-m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[1][2] * m.m[2][3] * m.m[3][0] - m.m[1][3] * m.m[2][0] * m.m[3][2]
		+ m.m[1][3] * m.m[2][2] * m.m[3][0] + m.m[1][2] * m.m[2][0] * m.m[3][3] + m.m[1][0] * m.m[2][3] * m.m[3][2]);
	m4.m[1][1] = 1 / m4s * (+m.m[0][0] * m.m[2][2] * m.m[3][3] + m.m[0][2] * m.m[2][3] * m.m[3][0] + m.m[0][3] * m.m[2][0] * m.m[3][2]
		- m.m[0][3] * m.m[2][2] * m.m[3][0] - m.m[0][2] * m.m[2][0] * m.m[3][3] - m.m[0][0] * m.m[2][3] * m.m[3][2]);
	m4.m[1][2] = 1 / m4s * (-m.m[0][0] * m.m[1][2] * m.m[3][3] - m.m[0][2] * m.m[1][3] * m.m[3][0] - m.m[0][3] * m.m[1][0] * m.m[3][2]
		+ m.m[0][3] * m.m[1][2] * m.m[3][0] + m.m[0][2] * m.m[1][0] * m.m[3][3] + m.m[0][0] * m.m[1][3] * m.m[3][2]);
	m4.m[1][3] = 1 / m4s * (m.m[0][0] * m.m[1][2] * m.m[2][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] + m.m[0][3] * m.m[1][0] * m.m[2][2]
		- m.m[0][3] * m.m[1][2] * m.m[2][0] - m.m[0][2] * m.m[1][0] * m.m[2][3] - m.m[0][0] * m.m[1][3] * m.m[2][2]);



	m4.m[2][0] = 1 / m4s * (
		m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[1][3] * m.m[2][0] * m.m[3][1]
		- m.m[1][3] * m.m[2][1] * m.m[3][0] - m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[1][0] * m.m[2][3] * m.m[3][1]
		);
	m4.m[2][1] = 1 / m4s * (
		-m.m[0][0] * m.m[2][1] * m.m[3][3] - m.m[0][1] * m.m[2][3] * m.m[3][0] - m.m[0][3] * m.m[2][0] * m.m[3][1]
		+ m.m[0][3] * m.m[2][1] * m.m[3][0] + m.m[0][1] * m.m[2][0] * m.m[3][3] + m.m[0][0] * m.m[2][3] * m.m[3][1]);
	m4.m[2][2] = 1 / m4s * (
		m.m[0][0] * m.m[1][1] * m.m[3][3] + m.m[0][1] * m.m[1][3] * m.m[3][0] + m.m[0][3] * m.m[1][0] * m.m[3][1]
		- m.m[0][3] * m.m[1][1] * m.m[3][0] - m.m[0][1] * m.m[1][0] * m.m[3][3] - m.m[0][0] * m.m[1][3] * m.m[3][1]);
	m4.m[2][3] = 1 / m4s * (
		-m.m[0][0] * m.m[1][1] * m.m[2][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] - m.m[0][3] * m.m[1][0] * m.m[2][1]
		+ m.m[0][3] * m.m[1][1] * m.m[2][0] + m.m[0][1] * m.m[1][0] * m.m[2][3] + m.m[0][0] * m.m[1][3] * m.m[2][1]);


	m4.m[3][0] = 1 / m4s * (-m.m[1][0] * m.m[2][1] * m.m[3][2] - m.m[1][1] * m.m[2][2] * m.m[3][0] - m.m[1][2] * m.m[2][0] * m.m[3][1]
		+ m.m[1][2] * m.m[2][1] * m.m[3][0] + m.m[1][1] * m.m[2][0] * m.m[3][2] + m.m[1][0] * m.m[2][2] * m.m[3][1]);
	m4.m[3][1] = 1 / m4s * (+m.m[0][0] * m.m[2][1] * m.m[3][2] + m.m[0][1] * m.m[2][2] * m.m[3][0] + m.m[0][2] * m.m[2][0] * m.m[3][1]
		- m.m[0][2] * m.m[2][1] * m.m[3][0] - m.m[0][1] * m.m[2][0] * m.m[3][2] - m.m[0][0] * m.m[2][2] * m.m[3][1]);
	m4.m[3][2] = 1 / m4s * (-m.m[0][0] * m.m[1][1] * m.m[3][2] - m.m[0][1] * m.m[1][2] * m.m[3][0] - m.m[0][2] * m.m[1][0] * m.m[3][1]
		+ m.m[0][2] * m.m[1][1] * m.m[3][0] + m.m[0][1] * m.m[1][0] * m.m[3][2] + m.m[0][0] * m.m[1][2] * m.m[3][1]);
	m4.m[3][3] = 1 / m4s * (m.m[0][0] * m.m[1][1] * m.m[2][2] + m.m[0][1] * m.m[1][2] * m.m[2][0] + m.m[0][2] * m.m[1][0] * m.m[2][1]
		- m.m[0][2] * m.m[1][1] * m.m[2][0] - m.m[0][1] * m.m[1][0] * m.m[2][2] - m.m[0][0] * m.m[1][2] * m.m[2][1]);

	return m4;

}

Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
	Matrix4x4 result;
	result.m[0][0] = 2.0f / (right - left);
	result.m[0][1] = 0;
	result.m[0][2] = 0;
	result.m[0][3] = 0;

	result.m[1][0] = 0;
	result.m[1][1] = 2.0f / (top - bottom);
	result.m[1][2] = 0;
	result.m[1][3] = 0;

	result.m[2][0] = 0;
	result.m[2][1] = 0;
	result.m[2][2] = 1.0f / (farClip - nearClip);
	result.m[2][3] = 0;

	result.m[3][0] = (left + right) / (left - right);
	result.m[3][1] = (top + bottom) / (bottom - top);
	result.m[3][2] = nearClip / (nearClip - farClip);
	result.m[3][3] = 1;

	return result;
}

Vector3 Normalize(const Vector3& v1) {
	Vector3 Result = v1;
	float length = sqrt(v1.x * v1.x + v1.y * v1.y + v1.z * v1.z);
	Result.x /= length;
	Result.y /= length;
	Result.z /= length;
	return Result;
}

Matrix4x4 MakeScaleMatrix(const Vector3& scale) {
	Matrix4x4 result{};
	result.m[0][0] = scale.x;
	result.m[0][1] = 0.0f;
	result.m[0][2] = 0.0f;
	result.m[0][3] = 0.0f;

	result.m[1][0] = 0.0f;
	result.m[1][1] = scale.y;
	result.m[1][2] = 0.0f;
	result.m[1][3] = 0.0f;

	result.m[2][0] = 0.0f;
	result.m[2][1] = 0.0f;
	result.m[2][2] = scale.z;
	result.m[2][3] = 0.0f;

	result.m[3][0] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[3][2] = 0.0f;
	result.m[3][3] = 1.0f;
	return result;
}

Matrix4x4 MakeTranslateMatrix(const Vector3& translate) {
	Matrix4x4 result{};
	result.m[0][0] = 1.0f;
	result.m[0][1] = 0.0f;
	result.m[0][2] = 0.0f;
	result.m[0][3] = 0.0f;

	result.m[1][0] = 0.0f;
	result.m[1][1] = 1.0f;
	result.m[1][2] = 0.0f;
	result.m[1][3] = 0.0f;

	result.m[2][0] = 0.0f;
	result.m[2][1] = 0.0f;
	result.m[2][2] = 1.0f;
	result.m[2][3] = 0.0f;

	result.m[3][0] = translate.x;
	result.m[3][1] = translate.y;
	result.m[3][2] = translate.z;
	result.m[3][3] = 1.0f;
	return result;
}

Matrix4x4 MakeRotateAxisAngle(const Vector3& axis, float angle)
{
	Matrix4x4 result;

	result.m[0][0] = (axis.x * axis.x) * (1 - std::cosf(angle)) + std::cosf(angle);
	result.m[0][1] = (axis.x * axis.y) * (1 - std::cosf(angle)) + (axis.z * std::sinf(angle));
	result.m[0][2] = (axis.x * axis.z) * (1 - std::cosf(angle)) - (axis.y * std::sinf(angle));
	result.m[0][3] = 0.0f;

	result.m[1][0] = (axis.x * axis.y) * (1 - std::cosf(angle)) - (axis.z * std::sinf(angle));
	result.m[1][1] = (axis.y * axis.y) * (1 - std::cosf(angle)) + std::cosf(angle);
	result.m[1][2] = (axis.y * axis.z) * (1 - std::cosf(angle)) + (axis.x * std::sinf(angle));
	result.m[1][3] = 0.0f;

	result.m[2][0] = (axis.x * axis.z) * (1 - std::cosf(angle)) + (axis.y * std::sinf(angle));
	result.m[2][1] = (axis.y * axis.z) * (1 - std::cosf(angle)) - (axis.x * std::sinf(angle));
	result.m[2][2] = (axis.z * axis.z) * (1 - std::cosf(angle)) + std::cosf(angle);
	result.m[2][3] = 0.0f;

	result.m[3][0] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[3][2] = 0.0f;
	result.m[3][3] = 1.0f;

	return result;
}

Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs)
{
	Quaternion result;

	result.x = lhs.y * rhs.z - lhs.z * rhs.y + rhs.w * lhs.x + lhs.w * rhs.x;
	result.y = lhs.z * rhs.x - lhs.x * rhs.z + rhs.w * lhs.y + lhs.w * rhs.y;
	result.z = lhs.x * rhs.y - lhs.y * rhs.x + rhs.w * lhs.z + lhs.w * rhs.z;
	result.w = lhs.w * rhs.w - Dot(Vector3{ lhs.x,lhs.y ,lhs.z }, { rhs.x,rhs.y ,rhs.z });

	return result;
}

float Dot(const Quaternion& q0, const Quaternion& q1)
{
	float result;

	result = q0.x * q1.x + q0.y * q1.y + q0.z * q1.z + q0.w * q1.w;

	return result;
}

Quaternion IdentityQuaternion()
{
	Quaternion result;
	result.x *= 0.0f;
	result.y *= 0.0f;
	result.z *= 0.0f;
	result.w *= 1.0f;
	return result;
}

Quaternion Conjugate(const Quaternion& quaternion)
{
	Quaternion result = quaternion;

	result.x *= -1.0f;
	result.y *= -1.0f;
	result.z *= -1.0f;
	result.w *= 1.0f;

	return result;
}

float Norm(const Quaternion& quaternion)
{
	float result = sqrt(quaternion.x * quaternion.x + quaternion.y * quaternion.y + quaternion.z * quaternion.z + quaternion.w * quaternion.w);

	return result;
}

Quaternion Normalize(const Quaternion& quaternion)
{
	Quaternion result;
	float len = 1 / sqrtf(quaternion.x * quaternion.x + quaternion.y * quaternion.y + quaternion.z * quaternion.z + quaternion.w * quaternion.w);
	result = { quaternion.x * len,quaternion.y * len,quaternion.z * len,quaternion.w * len };
	return result;
}

Quaternion Inverse(const Quaternion& quaternion)
{
	Quaternion result;

	float n = Norm(quaternion) * Norm(quaternion);
	Quaternion result2 = Conjugate(quaternion);


	result.x = result2.x / n;
	result.y = result2.y / n;
	result.z = result2.z / n;
	result.w = result2.w / n;

	return result;
}

Quaternion mainasu(const Quaternion& quaternion)
{
	Quaternion result = quaternion;

	result.x *= -1.0f;
	result.y *= -1.0f;
	result.z *= -1.0f;
	result.w *= -1.0f;

	return result;
}

Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle)
{
	Quaternion result;

	result.x = axis.x * std::sinf(angle / 2.0f);
	result.y = axis.y * std::sinf(angle / 2.0f);
	result.z = axis.z * std::sinf(angle / 2.0f);
	result.w = std::cosf(angle / 2.0f);

	return result;
}

Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion)
{
	Vector3 result;
	Quaternion n;
	n.x = vector.x;
	n.y = vector.y;
	n.z = vector.z;
	n.w = 0.0f;

	Quaternion a = Multiply(Multiply(quaternion, n), Conjugate(quaternion));

	result.x = a.x;
	result.y = a.y;
	result.z = a.z;

	return result;
}

Matrix4x4 MakeRotateMatrix(const Quaternion quaternion)
{
	Matrix4x4 result;

	result.m[0][0] = (quaternion.w * quaternion.w) + (quaternion.x * quaternion.x) - (quaternion.y * quaternion.y) - (quaternion.z * quaternion.z);
	result.m[0][1] = 2.0f * ((quaternion.x * quaternion.y) + (quaternion.w * quaternion.z));
	result.m[0][2] = 2.0f * ((quaternion.x * quaternion.z) - (quaternion.w * quaternion.y));
	result.m[0][3] = 0.0f;

	result.m[1][0] = 2.0f * ((quaternion.x * quaternion.y) - (quaternion.w * quaternion.z));
	result.m[1][1] = (quaternion.w * quaternion.w) - (quaternion.x * quaternion.x) + (quaternion.y * quaternion.y) - (quaternion.z * quaternion.z);
	result.m[1][2] = 2.0f * ((quaternion.y * quaternion.z) + (quaternion.w * quaternion.x));
	result.m[1][3] = 0.0f;

	result.m[2][0] = 2.0f * ((quaternion.x * quaternion.z) + (quaternion.w * quaternion.y));
	result.m[2][1] = 2.0f * ((quaternion.y * quaternion.z) - (quaternion.w * quaternion.x));
	result.m[2][2] = (quaternion.w * quaternion.w) - (quaternion.x * quaternion.x) - (quaternion.y * quaternion.y) + (quaternion.z * quaternion.z);
	result.m[2][3] = 0.0f;

	result.m[3][0] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[3][2] = 0.0f;
	result.m[3][3] = 1.0f;

	return result;
}

Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t)
{
	Quaternion result;

	float dot = Dot(q0, q1);
	if (dot < 0) {
		mainasu(q0);
		dot = -dot;
	}

	float theta = std::acos(dot);

	float scale0 = std::sin((1 - t) * theta) / std::sin(theta);
	float scale1 = std::sin(t * theta) / std::sin(theta);

	result.x = scale0 * q0.x + scale1 * q1.x;
	result.y = scale0 * q0.y + scale1 * q1.y;
	result.z = scale0 * q0.z + scale1 * q1.z;
	result.w = scale0 * q0.w + scale1 * q1.w;

	return result;
}