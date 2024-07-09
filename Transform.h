#pragma once
#include "Matrix4x4.h"
#include "Vector3.h"

Matrix4x4 MakeScaleMatrix(const Vector3& scale);
Matrix4x4 Multiply(Matrix4x4 matrix1, Matrix4x4 matrix2);
Matrix4x4 MakeScaleMatrix(const Vector3& scale);
Matrix4x4 MakeTranslateMatrix(const Vector3& translate);
Matrix4x4 MakeRotateXMatrix(float radian);
Matrix4x4 MakeRotateYMatrix(float radian);
Matrix4x4 MakeRotateZMatrix(float radian);
Matrix4x4 MakeAffineMatrix(const Vector3& scale,
	const Vector3& rotate, const Vector3& translation);

Matrix4x4 MakePerspectiveForMatrix(float fovY, float aspectRatio, float nearClip, float farClip);
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);


Matrix4x4 MakeIdentity4x4();


struct Transform { //02_02 p14
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};
