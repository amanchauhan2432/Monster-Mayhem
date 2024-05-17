#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../AmmoType.h"
#include "Item.generated.h"

UENUM(BlueprintType)
enum class EItemState : uint8
{
	EIS_Pickup UMETA(DisplayName = "Pickup"),
	EIS_EquipInterping UMETA(DisplayName = "EquipInterping"),
	EIS_Pickedup UMETA(DisplayName = "Pickedup"),
	EIS_Equipped UMETA(DisplayName = "Equipped"),
	EIS_Falling UMETA(DisplayName = "Falling"),

	EIS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class MONSTER_MAYHEM_API AItem : public AActor
{
	GENERATED_BODY()
	
public:
	AItem();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USphereComponent* CollisionSphere;

	UPROPERTY(EditAnywhere)
	class UWidgetComponent* PickupWidget;

	class AStella* OverlappingPlayer;
	AStella* Character;

	EItemState ItemState { EItemState::EIS_Pickup };

	FTimerHandle WeaponThrowHandle;
	FRotator ThrowRotation;

	UPROPERTY(EditDefaultsOnly, Category = "Curves")
	class UCurveFloat* ItemZCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundBase* PickupSound;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundBase* EquipSound;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Properties")
	int32 ItemCount{ 30 };

	FTimerHandle InterpTimerHandle;
	bool bCanInterp;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void SetItemState(EItemState InState);
	virtual void SetItemProperties(EItemState InState);

	void ThrowWeapon();
	void StopFalling();

	void StartInterpolation(AStella* InCharacter);
	void FinishInterpolation();
	void InterpItem(float DeltaTime);
};
