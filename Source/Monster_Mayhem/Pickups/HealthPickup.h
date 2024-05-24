#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HealthPickup.generated.h"

UCLASS()
class MONSTER_MAYHEM_API AHealthPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	AHealthPickup();

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* PickupMesh;

	UPROPERTY(EditAnywhere)
	class USphereComponent* SphereCollision;

	UFUNCTION()
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	virtual void BeginPlay() override;

};
