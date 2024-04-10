#include "CollisionManager.h"

void CollisionManeger::CheckCollisionPair(Collider* colliderA, Collider* colliderB) {
	// 判定対象AとBの座標と半径
	Vector3 posA, posB;
	float radiusA, radiusB;

	// colliderAの座標と半径
	posA = colliderA->GetWorldPosition();
	radiusA = colliderA->GetRadius();

	// colliderBの座標と半径
	posB = colliderB->GetWorldPosition();
	radiusB = colliderB->GetRadius();

	// 弾と弾の交差判定
	float p2b = (posB.x - posA.x) * (posB.x - posA.x) + (posB.y - posA.y) * (posB.y - posA.y) +
		(posB.z - posA.z) * (posB.z - posA.z);

	float r2r = (radiusA + radiusB) * (radiusA + radiusB);

	if (p2b <= r2r) {
		// 衝突フィルタリング
		if (colliderA->GetCollisionAttribute() != colliderB->GetCollisionMask() ||
			colliderB->GetCollisionAttribute() != colliderA->GetCollisionMask()) {
			return;
		};
		// コライダーAの衝突時コールバックを呼び出す
		colliderA->OnCollision();
		// コライダーBの衝突時コールバックを呼び出す
		colliderB->OnCollision();
	}
}

void CollisionManeger::CheckAllCollision() {
	// 自弾リストの取得
	const std::list<PlayerBullet*>& playerBullets = player_->GetBullets();
	// 敵弾リストの取得
	const std::list<EnemyBullet*>& enemyBullets = enemy_->GetBullets();
	// コライダー
	std::list<Collider*> colliders_;

	// コライダーをリストに登録
	//colliders_.push_back(player_);
	//olliders_.push_back(enemy_);
	// 自弾全てについて
	for (PlayerBullet* bullet : playerBullets) {
		//colliders_.push_back(bullet);
	}
	// 敵弾全てについて
	for (EnemyBullet* bullet : enemyBullets) {
		//colliders_.push_back(bullet);
	}

	// リスト内のペアを総当たり
	std::list<Collider*>::iterator itrA = colliders_.begin();
	for (; itrA != colliders_.end(); ++itrA) {

		// イテレータBはイテレータ―Aの次の要素から回す（重複判定を回避）
		std::list<Collider*>::iterator itrB = itrA;
		itrB++;
		for (; itrB != colliders_.end(); ++itrB) {

			// ペアの当たり判定
			CheckCollisionPair(*itrA, *itrB);
		}
	}
}