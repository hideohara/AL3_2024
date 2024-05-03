#pragma once

#include "Matrix4x4.h"
#include "Vector3.h"


// 円周率
const float PI = 3.141592654f;

// 平行移動行列の作成
Matrix4x4 MakeTranslateMatrix(const Vector3& translate);

// アフィン変換行列の作成
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rot, const Vector3& translate);

// 掛け算
Matrix4x4 MatrixMultiply(Matrix4x4& m1, Matrix4x4& m2);

// 代入演算子オーバーロード
Vector3& operator+=(Vector3& lhs, const Vector3& rhv);

float EaseInOut(float x1, float x2, float t);

float Lerp(float x1, float x2, float t);
