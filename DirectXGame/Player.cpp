#define NOMINMAX

#include "Player.h"
#include <cassert>
#include <numbers>
#include <algorithm>
#include "MapChipField.h"

// プレイヤー

void Player::Initialize(Model* model, uint32_t textureHandle, ViewProjection* viewProjection, const Vector3& position)
{

	// NULLポインタチェック
	assert(model);

	//引数として受け取ったデータをメンバ変数に記録する
	model_ = model;
	textureHandle_ = textureHandle;
	viewProjection_ = viewProjection;
	worldTransform_.translation_ = position;

	// ワールド変換の初期化
	worldTransform_.Initialize();


	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;


}

void Player::Update()
{
	// ①移動入力
	InputMove();

	// 衝突情報を初期化
	CollisionMapInfo collisionMapInfo;
	// 移動量に速度の値をコピー
	collisionMapInfo.move = velocity_;

	// マップ衝突チェック
	CheckMapCollision(collisionMapInfo);

	//③判定結果を反映して移動させる
	// 移動
	worldTransform_.translation_ += collisionMapInfo.move;


	// 天井に当たった？
	if (collisionMapInfo.ceiling) {
		velocity_.y = 0;
	}



	// 壁接触による減速
	if (collisionMapInfo.hitWall) {
		velocity_.x *= (1.0f - kAttenuationWall);
	}

	UpdateOnGround(collisionMapInfo);


	/*
	// 着地フラグ
	bool landing = false;

	// 地面との当たり判定
	// 下降中？
	if (velocity_.y < 0) {
		// Y座標が地面以下になったら着地
		if (worldTransform_.translation_.y <= 1.0f) {
			landing = true;
		}
	}



	// 接地判定
	if (onGround_) {
		// ジャンプ開始
		if (velocity_.y > 0.0f) {
			// 空中状態に移行
			onGround_ = false;
		}
	}
	else {
		// 着地
		if (landing) {
			// めり込み排斥
			worldTransform_.translation_.y = 1.0f;
			// 摩擦で横方向速度が減衰する
			velocity_.x *= (1.0f - kAttenuationLanding);
			// 下方向速度をリセット
			velocity_.y = 0.0f;
			// 接地状態に移行
			onGround_ = true;
		}
	}
	*/


	// 移動
	//worldTransform_.translation_ += velocity_;

	// 行列を更新
	worldTransform_.UpdateMatrix();


}

void Player::Draw()
{
	// 3Dモデルを描画
	//model_->Draw(worldTransform_, *viewProjection_, textureHandle_);
	model_->Draw(worldTransform_, *viewProjection_);
}

void Player::InputMove()
{
	if (onGround_) {
		// 移動入力
		// 左右移動操作
		if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {

			// 左右加速
			Vector3 acceleration = {};
			if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
				// 左移動中の右入力
				if (velocity_.x < 0.0f) {
					// 速度と逆方向に入力中は急ブレーキ
					velocity_.x *= (1.0f - kAttenuation);
				}
				acceleration.x += kAcceleration;
				// 角度表示
				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
			}
			else if (Input::GetInstance()->PushKey(DIK_LEFT)) {
				// 右移動中の左入力
				if (velocity_.x > 0.0f) {
					// 速度と逆方向に入力中は急ブレーキ
					velocity_.x *= (1.0f - kAttenuation);
				}

				acceleration.x -= kAcceleration;
				// 角度表示
				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
			}


			// 加速／減速
			velocity_ += acceleration;
		}
		else {
			// 非入力時は移動減衰をかける
			velocity_.x *= (1.0f - kAttenuation);
		}

		// 旋回制御
		if (turnTimer_ > 0.0f)
		{
			//旋回タイマーを1 / 60秒分カウントダウンする
			turnTimer_ -= 1.0f / 60.0f;
			//turnTimer_ = std::max(turnTimer_ - (1.0f / 60.0f), 0.0f);

				// 左右の自キャラ角度テーブル
			float destinationRotationYTable[] = {
				std::numbers::pi_v<float> / 2.0f,
				std::numbers::pi_v<float> *3.0f / 2.0f
			};
			// 状態に応じた目標角度を取得する
			float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
			// 自キャラの角度を設定する
			// 旋回タイマーを使って角度補間;
			worldTransform_.rotation_.y = EaseInOut(destinationRotationY, turnFirstRotationY_, turnTimer_ / kTimeTurn);

		}

		if (Input::GetInstance()->PushKey(DIK_UP)) {
			// ジャンプ初速
			velocity_ += Vector3(0, kJumpAcceleration, 0);
			//velocity_ += Vector3(0, kJumpAcceleration / 60.0f, 0);
		}
	}
	// 空中
	else {
		// 落下速度
		velocity_ += Vector3(0, -kGravityAcceleration, 0);
		// 落下速度制限
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}

}

void Player::CheckMapCollision(CollisionMapInfo& info)
{
	CheckMapCollisionUp(info);
	CheckMapCollisionDown(info);
	CheckMapCollisionRight(info);
	CheckMapCollisionLeft(info);
}

void Player::CheckMapCollisionUp(CollisionMapInfo& info)
{
	// 上昇あり？
	if (info.move.y <= 0) {
		return;
	}

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	// 真上の当たり判定を行う
	bool hit = false;
	// 左上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 右上点の判定
	//kRightTopについて同様に判定する
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0, +kHeight / 2.0f, 0));
		// めり込み先ブロックの範囲矩形
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.y = (std::max)(0.0f, rect.bottom - worldTransform_.translation_.y - (kHeight / 2.0f + kBlank));

		// 天井に当たったことを記録する
		info.ceiling = true;
	}


}

void Player::CheckMapCollisionDown(CollisionMapInfo& info)
{
	// 下降あり？
	if (info.move.y >= 0) {
		return;
	}


	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	// 真下の当たり判定を行う
	bool hit = false;
	// 左下点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0, -kHeight / 2.0f, 0));
		// めり込み先ブロックの範囲矩形
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		//info.move.y = std::min(0.0f, Y移動量);
		info.move.y = std::min(0.0f, rect.top - worldTransform_.translation_.y + (kHeight / 2.0f + kBlank));
		//info.move.y = (std::max)(0.0f, rect.bottom - worldTransform_.translation_.y - (kHeight / 2.0f + kBlank));
		// 地面に当たったことを記録する
		info.landing = true;
	}


}


void Player::CheckMapCollisionRight(CollisionMapInfo& info)
{
	// 移動あり？
	if (info.move.x <= 0) {
		return;
	}

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	// 右の当たり判定を行う
	bool hit = false;
	// 右上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	//// ブロックにヒット？
	//if (hit) {
	//	// めり込みを排除する方向に移動量を設定する
	//	indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(+kHeight / 2.0f, 0, 0));
	//	// めり込み先ブロックの範囲矩形
	//	MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
	//	//info.move.x = (std::max)(rect.left - worldTransform_.translation_.x - (kWidth / 2.0f + kBlank), 0.0f);
	//	//info.move.x = (std::max)(0.0f, rect.bottom - worldTransform_.translation_.x - (kWidth / 2.0f + kBlank));
	//	info.move.x = std::max(rect.left - worldTransform_.translation_.x - (kWidth / 2.0f + kBlank), 0.0f);

	//	// 壁に当たったことを記録する
	//	info.hitWall = true;
	//}

		// ブロックにヒット？
	//if (hit) {
	//	// 現在座標が壁の外か判定
	//	MapChipField::IndexSet indexSetNow;
	//	indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(+kWidth / 2.0f, 0, 0));
	//	if (indexSetNow.xIndex != indexSet.xIndex) {
	//		// めり込みを排除する方向に移動量を設定する
	//		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(+kWidth / 2.0f, 0, 0));
	//		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
	//		info.move.x = std::max(rect.left - worldTransform_.translation_.x - (kWidth / 2.0f + kBlank), 0.0f);
	//		info.hitWall = true;
	//	}
	//}


	// ブロックにヒット？
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(+kHeight / 2.0f, 0, 0));
		// めり込み先ブロックの範囲矩形
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.x = (std::max)(0.0f, rect.left - worldTransform_.translation_.x - (kWidth / 2.0f + kBlank));

		// 壁に当たったことを記録する
		info.hitWall = true;
	}

}



void Player::CheckMapCollisionLeft(CollisionMapInfo& info)
{
	// 移動あり？
	if (info.move.x >= 0) {
		return;
	}

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	// 左の当たり判定を行う
	bool hit = false;
	// 左上点の判定
	MapChipField::IndexSet indexSet;

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 左下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(+kHeight / 2.0f, 0, 0));
		// めり込み先ブロックの範囲矩形
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		//info.move.x = (std::max)(rect.left - worldTransform_.translation_.x - (kWidth / 2.0f + kBlank), 0.0f);
		info.move.x = (std::max)(0.0f, rect.bottom - worldTransform_.translation_.x - (kWidth / 2.0f + kBlank));

		// 壁に当たったことを記録する
		info.hitWall = true;
	}

}

void Player::UpdateOnGround(CollisionMapInfo& info)
{

	// 自キャラが接地状態？
	if (onGround_) {

		//接地状態の処理

		// ジャンプ開始
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		}
		else {

			std::array<Vector3, kNumCorner> positionsNew;

			for (uint32_t i = 0; i < positionsNew.size(); ++i) {
				positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
			}

			bool ground = false;

			//MapChipType mapChipType;
			//// 左下点の判定
			//MapChipField::IndexSet indexSet;
			//indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom] + Vector3(0, -kGroundSearchHeight, 0));
			//mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			//if (mapChipType == MapChipType::kBlock) {
			//	ground = true;
			//}
			//// 右下点の判定
			//indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom] + Vector3(0, -kGroundSearchHeight, 0));
			//mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			//if (mapChipType == MapChipType::kBlock) {
			//	ground = true;
			//}

			// 落下開始
			if (!ground) {
			//	DebugText::GetInstance()->ConsolePrintf("jump");
				onGround_ = false;
			}
		}
	}
	else {

		//空中状態の処理

		// 着地フラグ
		if (info.landing) {
			// 着地状態に切り替える（落下を止める）
			onGround_ = true;
			// 着地時にX速度を減衰
			velocity_.x *= (1.0f - kAttenuationLanding);
			// Y速度をゼロにする
			velocity_.y = 0.0f;
		}

	}


}

Vector3 Player::CornerPosition(const Vector3& center, Corner corner)
{
	Vector3 offsetTable[kNumCorner] = {
		{+kWidth / 2.0f, -kHeight / 2.0f, 0}, // kRightBottom
		{-kWidth / 2.0f, -kHeight / 2.0f, 0}, // kLeftBottom
		{+kWidth / 2.0f, +kHeight / 2.0f, 0}, // kRightTop
		{-kWidth / 2.0f, +kHeight / 2.0f, 0}  // kLeftTop
	};

	return center + offsetTable[static_cast<uint32_t>(corner)];


}
