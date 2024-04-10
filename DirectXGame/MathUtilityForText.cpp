#include "MathUtilityForText.h"

Matrix4x4 MakeTranslateMatrix(const Vector3& translate) {
	Matrix4x4 result{ 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,        1.0f,        0.0f,        0.0f,
					 0.0f, 0.0f, 1.0f, 0.0f, translate.x, translate.y, translate.z, 1.0f };

	return result;
}

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rot, const Vector3& translate)
{
	// エラー対策
	Vector3 scale2 = scale;
	Vector3 rot2 = rot;

	// 平行移動行列の作成
	Matrix4x4 matTrans = MakeTranslateMatrix(translate);

	// とりあえず平行移動行列だけ返す
	return matTrans;
}

