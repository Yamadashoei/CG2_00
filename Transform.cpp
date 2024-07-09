#include "Matrix4x4.h"
#include "Vector3.h"
#include <cmath>
#include "Transform.h"


Matrix4x4 Multiply(Matrix4x4 matrix1, Matrix4x4 matrix2) {
	Matrix4x4 result = {};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++) {
				result.m[i][j] += matrix1.m[i][k] * matrix2.m[k][j];
			}
		}
	}
	return result;
}


// 拡大縮小行列
Matrix4x4 MakeScaleMatrix(const Vector3& scale) {
	Matrix4x4 ans = { 0 };
	ans.m[0][0] = scale.x;
	ans.m[1][1] = scale.y;
	ans.m[2][2] = scale.z;
	ans.m[3][3] = 1;
	return ans;
}

// 平行移動行列
Matrix4x4 MakeTranslateMatrix(const Vector3& translate) {
	Matrix4x4 ans = { 0 };
	ans.m[0][0] = 1;
	ans.m[1][1] = 1;
	ans.m[2][2] = 1;
	ans.m[3][0] = translate.x;
	ans.m[3][1] = translate.y;
	ans.m[3][2] = translate.z;
	ans.m[3][3] = 1;
	return ans;
}

//X
Matrix4x4 MakeRotateXMatrix(float radian) {
	Matrix4x4 ans = { 0 };
	ans.m[0][0] = 1;
	ans.m[1][1] = cosf(radian);
	ans.m[1][2] = sinf(radian);
	ans.m[2][1] = -sinf(radian);
	ans.m[2][2] = cosf(radian);
	ans.m[3][3] = 1;
	return ans;
}
//Y
Matrix4x4 MakeRotateYMatrix(float radian) {
	Matrix4x4 ans = { 0 };
	ans.m[0][0] = cosf(radian);
	ans.m[0][2] = -sinf(radian);
	ans.m[1][1] = 1;
	ans.m[2][0] = sinf(radian);
	ans.m[2][2] = cosf(radian);
	ans.m[3][3] = 1;
	return ans;
}
//Z
Matrix4x4 MakeRotateZMatrix(float radian) {
	Matrix4x4 ans = { 0 };
	ans.m[0][0] = cosf(radian);
	ans.m[0][1] = sinf(radian);
	ans.m[1][0] = -sinf(radian);
	ans.m[1][1] = cosf(radian);
	ans.m[2][2] = 1;
	ans.m[3][3] = 1;
	return ans;
}

//3次元アフィン変換行列
Matrix4x4 MakeAffineMatrix(const Vector3& scale,
	const Vector3& rotate, const Vector3& translation) {
	Matrix4x4 ans;
	ans = Multiply(Multiply(MakeScaleMatrix(scale), Multiply(Multiply(MakeRotateXMatrix(rotate.x), MakeRotateYMatrix(rotate.y)), MakeRotateZMatrix(rotate.z))), MakeTranslateMatrix(translation));
	return ans;
}


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

//透視投影行列 04_00 p4
Matrix4x4 MakePerspectiveForMatrix(float fovY, float aspectRatio, float nearClip, float farClip)
{
	Matrix4x4 result;
	float cot = 1.0f / std::tan(fovY / 2.0f);

	result.m[0][0] = cot / aspectRatio;
	result.m[0][1] = 0.0f;
	result.m[0][2] = 0.0f;
	result.m[0][3] = 0.0f;

	result.m[1][0] = 0.0f;
	result.m[1][1] = cot;
	result.m[1][2] = 0.0f;
	result.m[1][3] = 0.0f;

	result.m[2][0] = 0.0f;
	result.m[2][1] = 0.0f;
	result.m[2][2] = farClip / (farClip - nearClip);
	result.m[2][3] = -1.0f;

	result.m[3][1] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[3][2] = (-nearClip * farClip) / (farClip - nearClip);
	result.m[3][3] = 0.0f;

	return result;
}

Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip)
{
	Matrix4x4 ans = { 0 };
	ans.m[0][0] = 2 / (right - left);
	ans.m[1][1] = 2 / (top - bottom);
	ans.m[2][2] = 1 / (farClip - nearClip);
	ans.m[3][0] = (left + right) / (left - right);
	ans.m[3][1] = (top + bottom) / (bottom - top);
	ans.m[3][2] = nearClip / (nearClip - farClip);
	ans.m[3][3] = 1;
	return ans;
}
