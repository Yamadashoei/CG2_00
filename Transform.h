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


Matrix4x4 MakeIdentity4x4() {
	Matrix4x4 ans = { 0 };
	for (int a = 0; a < 4; a++) {
		for (int b = 0; b < 4; b++) {
			if (a == b) {
				ans.m[a][b] = 1;
			}
		}
	}
	return ans;
}


struct Transform { //02_02 p14
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};
