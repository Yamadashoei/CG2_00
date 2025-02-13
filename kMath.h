#pragma once

#include"Vector2.h"
#include"Vector3.h"
#include"Vector4.h"
#include"Matrix4x4.h"
#include"Transform.h"

#define _USE_MATH_DEFINES
#include <math.h>

namespace kMath {
	// 行列を作成する関数
#pragma region Affine
// スケール行列を生成
	Matrix4x4 MakeScaleMatrix(const Vector3& scale);
	// 2つの行列を乗算
	Matrix4x4 Multiply(Matrix4x4 matrix1, Matrix4x4 matrix2);
	// 平行移動行列を生成
	Matrix4x4 MakeTranslateMatrix(const Vector3& translate);
	// X軸回転行列を生成
	Matrix4x4 MakeRotateXMatrix(float radian);
	// Y軸回転行列を生成
	Matrix4x4 MakeRotateYMatrix(float radian);
	// Z軸回転行列を生成
	Matrix4x4 MakeRotateZMatrix(float radian);
	// アフィン変換行列を生成（スケール、回転、平行移動を組み合わせた行列）
	Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translation);
#pragma endregion

	// 射影行列を作成する関数
	// パースペクティブ射影行列を生成
	Matrix4x4 MakePerspectiveForMatrix(float fovY, float aspectRatio, float nearClip, float farClip);
	// 正射影行列を生成
	Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);

#pragma region 逆数
	// 行列操作のユーティリティ
	// 行列の逆行列を計算
	Matrix4x4 Inverse(const Matrix4x4& m);
#pragma endregion

	// 単位行列（4x4）を生成
	Matrix4x4 MakeIdentity4x4();

}

