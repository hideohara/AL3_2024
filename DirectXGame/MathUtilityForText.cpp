#include "MathUtilityForText.h"

Matrix4x4 MakeTranslateMatrix(const Vector3& translate) {
	Matrix4x4 result{ 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,        1.0f,        0.0f,        0.0f,
					 0.0f, 0.0f, 1.0f, 0.0f, translate.x, translate.y, translate.z, 1.0f };

	return result;
}

/*
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rot, const Vector3& translate)
{
	// エラー対策
	Vector3 scale2 = scale;
	Vector3 rot2 = rot;

	// 平行移動行列の作成
	//Matrix4x4 matTrans = MakeTranslateMatrix(translate);
	Matrix4x4 matTrans{ 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,        1.0f,        0.0f,        0.0f,
				 0.0f, 0.0f, 1.0f, 0.0f, translate.x, translate.y, translate.z, 1.0f };

	// とりあえず平行移動行列だけ返す
	return matTrans;
}
*/

// アフィン変換行列の作成
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rot, const Vector3& translate) {

	Matrix4x4 ansMat;
	Matrix4x4 IdentityMat = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	Matrix4x4 ScallMat = IdentityMat;
	Matrix4x4 RotateMat = IdentityMat;
	Matrix4x4 RotateMatX = IdentityMat;
	Matrix4x4 RotateMatY = IdentityMat;
	Matrix4x4 RotateMatZ = IdentityMat;
	Matrix4x4 TranslateMat = IdentityMat;

	//スケール
	ScallMat = { scale.x, 0, 0, 0,
				0, scale.y, 0, 0,
				0, 0, scale.z, 0,
				0, 0, 0, 1 };


	//回転
	RotateMatX = { 1, 0, 0, 0,
				  0, cosf(rot.x), sinf(rot.x), 0,
				  0, -sinf(rot.x), cosf(rot.x),0,
				  0, 0, 0, 1 };

	RotateMatY = { cosf(rot.y), 0, -sinf(rot.y), 0,
				  0, 1, 0, 0,
				  sinf(rot.y), 0, cosf(rot.y),  0,
				  0, 0, 0, 1 };

	RotateMatZ = { cosf(rot.z), sinf(rot.z),0,0,
				 -sinf(rot.z), cosf(rot.z),0,0,
				  0,0,1,0,
				  0,0,0,1 };

	// Z軸回転＊X軸回転
	RotateMat = MatrixMultiply(RotateMatZ, RotateMatX);
	//↑の結果＊Y軸回転
	RotateMat = MatrixMultiply(RotateMat, RotateMatY);

	//平行移動行列の作成
	TranslateMat = { 1, 0, 0, 0,
					0, 1, 0, 0,
					0, 0, 1, 0,
					translate.x, translate.y, translate.z, 1
	};

	//スケール＊回転＊平行移動をワールド変換行列に
	ansMat = MatrixMultiply(ScallMat, RotateMat);
	ansMat = MatrixMultiply(ansMat, TranslateMat);

	return ansMat;
}

// 行列の掛け算
Matrix4x4 MatrixMultiply(Matrix4x4& m1, Matrix4x4& m2) {

	Matrix4x4 result;
	result.m[0][0] = m1.m[0][0] * m2.m[0][0] +
		m1.m[0][1] * m2.m[1][0] +
		m1.m[0][2] * m2.m[2][0] +
		m1.m[0][3] * m2.m[3][0];
	result.m[0][1] = m1.m[0][0] * m2.m[0][1] +
		m1.m[0][1] * m2.m[1][1] +
		m1.m[0][2] * m2.m[2][1] +
		m1.m[0][3] * m2.m[3][1];
	result.m[0][2] = m1.m[0][0] * m2.m[0][2] +
		m1.m[0][1] * m2.m[1][2] +
		m1.m[0][2] * m2.m[2][2] +
		m1.m[0][3] * m2.m[3][2];
	result.m[0][3] = m1.m[0][0] * m2.m[0][3] +
		m1.m[0][1] * m2.m[1][3] +
		m1.m[0][2] * m2.m[2][3] +
		m1.m[0][3] * m2.m[3][3];
	result.m[1][0] = m1.m[1][0] * m2.m[0][0] +
		m1.m[1][1] * m2.m[1][0] +
		m1.m[1][2] * m2.m[2][0] +
		m1.m[1][3] * m2.m[3][0];
	result.m[1][1] = m1.m[1][0] * m2.m[0][1] +
		m1.m[1][1] * m2.m[1][1] +
		m1.m[1][2] * m2.m[2][1] +
		m1.m[1][3] * m2.m[3][1];
	result.m[1][2] = m1.m[1][0] * m2.m[0][2] +
		m1.m[1][1] * m2.m[1][2] +
		m1.m[1][2] * m2.m[2][2] +
		m1.m[1][3] * m2.m[3][2];
	result.m[1][3] = m1.m[1][0] * m2.m[0][3] +
		m1.m[1][1] * m2.m[1][3] +
		m1.m[1][2] * m2.m[2][3] +
		m1.m[1][3] * m2.m[3][3];
	result.m[2][0] = m1.m[2][0] * m2.m[0][0] +
		m1.m[2][1] * m2.m[1][0] +
		m1.m[2][2] * m2.m[2][0] +
		m1.m[2][3] * m2.m[3][0];
	result.m[2][1] = m1.m[2][0] * m2.m[0][1] +
		m1.m[2][1] * m2.m[1][1] +
		m1.m[2][2] * m2.m[2][1] +
		m1.m[2][3] * m2.m[3][1];
	result.m[2][2] = m1.m[2][0] * m2.m[0][2] +
		m1.m[2][1] * m2.m[1][2] +
		m1.m[2][2] * m2.m[2][2] +
		m1.m[2][3] * m2.m[3][2];
	result.m[2][3] = m1.m[2][0] * m2.m[0][3] +
		m1.m[2][1] * m2.m[1][3] +
		m1.m[2][2] * m2.m[2][3] +
		m1.m[2][3] * m2.m[3][3];
	result.m[3][0] = m1.m[3][0] * m2.m[0][0] +
		m1.m[3][1] * m2.m[1][0] +
		m1.m[3][2] * m2.m[2][0] +
		m1.m[3][3] * m2.m[3][0];
	result.m[3][1] = m1.m[3][0] * m2.m[0][1] +
		m1.m[3][1] * m2.m[1][1] +
		m1.m[3][2] * m2.m[2][1] +
		m1.m[3][3] * m2.m[3][1];
	result.m[3][2] = m1.m[3][0] * m2.m[0][2] +
		m1.m[3][1] * m2.m[1][2] +
		m1.m[3][2] * m2.m[2][2] +
		m1.m[3][3] * m2.m[3][2];
	result.m[3][3] = m1.m[3][0] * m2.m[0][3] +
		m1.m[3][1] * m2.m[1][3] +
		m1.m[3][2] * m2.m[2][3] +
		m1.m[3][3] * m2.m[3][3];
	return result;
}

