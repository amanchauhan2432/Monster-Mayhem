#pragma once

#include "CoreMinimal.h"
#include "../Item.h"
#include "../../AmmoType.h"
#include "Ammo.generated.h"

UCLASS()
class MONSTER_MAYHEM_API AAmmo : public AItem
{
	GENERATED_BODY()
	
public:

	AAmmo();

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* AmmoMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Properties")
	EAmmoType AmmoType {EAmmoType::EAT_9mm};

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetItemProperties(EItemState InState) override;

	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
};
