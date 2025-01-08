#pragma once

class Collider {
public:
	// 衝突判定
	virtual void OnCollision() = 0;

	// Getter
	float GetRadius() { return radius_; }
	virtual Vector3 GetWorldPosition() const = 0;
	uint32_t GetCollosionAttribute() const { return collisionAttribute_; }
	uint32_t GetCollisionMask() const { return CollisionMask_; }
	Vector3 GetAABBMin() const { return { centerPosition.x - halfSize.x, centerPosition.y - halfSize.y, centerPosition.z - halfSize.z };}
	Vector3 GetAABBMax() const { return { centerPosition.x + halfSize.x, centerPosition.y + halfSize.y, centerPosition.z + halfSize.z };}
	Vector3 GetHalfSize() const { return halfSize; }

	// Settter
	void SetRadius(float radius) { radius_ = radius; }
	void SetCollosionAttribute(uint32_t collisionAttribute) { collisionAttribute_ = collisionAttribute; }
	void SetCollisionMask(uint32_t collisionMask) { CollisionMask_ = collisionMask; }
	void SetCenterPosition(Vector3 pos) { centerPosition = pos; }
	void SetHalfSize(Vector3 size) { halfSize = size; }
private:
	// 自機
	uint32_t collisionAttribute_ = 0xffffffff;
	// 相手
	uint32_t CollisionMask_ = 0x11111111;
	// 衝突半径
	float radius_ = 1.0f;
	// コライダーの中心位置（ワールド座標）
	Vector3 centerPosition;
	// AABBの半サイズ（各軸方向の範囲 / 2）
	Vector3 halfSize;
};