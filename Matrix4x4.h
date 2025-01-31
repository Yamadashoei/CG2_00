#pragma once
#include <cmath>

struct Matrix4x4 final {
    float m[4][4];

    static Matrix4x4 MakeMatrix4x4(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33)
    {
        Matrix4x4 matrix;
        matrix.m[0][0] = m00; matrix.m[0][1] = m01; matrix.m[0][2] = m02; matrix.m[0][3] = m03;
        matrix.m[1][0] = m10; matrix.m[1][1] = m11; matrix.m[1][2] = m12; matrix.m[1][3] = m13;
        matrix.m[2][0] = m20; matrix.m[2][1] = m21; matrix.m[2][2] = m22; matrix.m[2][3] = m23;
        matrix.m[3][0] = m30; matrix.m[3][1] = m31; matrix.m[3][2] = m32; matrix.m[3][3] = m33;
        return matrix;
    }
    static Matrix4x4 MakePerspectiveMatrix(float y, float a, float n, float f)
    {
        return Matrix4x4::MakeMatrix4x4(
            1 / (a * tanf(y / 2)), 0, 0, 0,
            0, 1 / tanf(y / 2), 0, 0,
            0, 0, f / (f - n), 1,
            0, 0, -n * f / (f - n), 0
        );
    }

    // 行列同士の積のオーバーロード
    Matrix4x4 operator*(const Matrix4x4& other) const {
        Matrix4x4 result = {}; // 初期化
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                result.m[i][j] = 0;
                for (int k = 0; k < 4; ++k) {
                    result.m[i][j] += m[i][k] * other.m[k][j];
                }
            }
        }
        return result;
    }

};