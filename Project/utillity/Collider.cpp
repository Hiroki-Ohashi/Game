#include "Collider.h"

// 衝突属性（自分）を設定
void Collider::SetCollisionAttribute(uint32_t collisionAttribute) {
	collisionAttribute_ = collisionAttribute;
};
// 衝突マスク（相手）を設定
void Collider::SetCollisionMask(uint32_t CollisionMask) {
	collisionMask_ = CollisionMask;
};