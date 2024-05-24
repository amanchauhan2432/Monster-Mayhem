#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Interfaces/BulletHitInterface.h"
#include "Explosive.generated.h"

UCLASS()
class MONSTER_MAYHEM_API AExplosive : public AActor, public IBulletHitInterface
{
	GENERATED_BODY()
	
public:
	AExplosive();

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* ExplosiveMesh;

	UPROPERTY(EditAnywhere)
	class USphereComponent* SphereCollision;

	UPROPERTY(EditDefaultsOnly)
	float BaseDamage = 25.f;

	UPROPERTY(EditAnywhere, Category = "Enemy Properties")
	class UParticleSystem* HitEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy Properties")
	USoundBase* HitSound;

	void BulletHit_Implementation(FVector HitLocation);
};
