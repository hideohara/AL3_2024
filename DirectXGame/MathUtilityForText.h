#pragma once

#include "Matrix4x4.h"
#include "Vector3.h"

// 平行移動行列の作成
Matrix4x4 MakeTranslateMatrix(const Vector3& translate);

// アフィン変換行列の作成
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rot, const Vector3& translate);

