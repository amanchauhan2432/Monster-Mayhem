#pragma once

#include "CoreMinimal.h"
#include "../Item.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_SMG UMETA(DisplayName = "SMG"),
	EWT_AR UMETA(DisplayName = "AR"),
	EWT_Pistol UMETA(DisplayName = "Pistol"),

	EWT_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class MONSTER_MAYHEM_API AWeapon : public AItem
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Properties")
	int32 Ammo = 30;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Properties")
	int32 MagazineCapacity = 30;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Properties")
	FString WeaponName{ "Submachine Gun" };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Properties")
	FName ClipBoneName{ "smg_clip" };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Properties")
	EWeaponType WeaponType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Properties")
	EAmmoType AmmoType = EAmmoType::EAT_9mm;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Properties")
	FName ReloadMontageSection{ "SMG" };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Properties")
	UTexture2D* WeaponIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Properties")
	UTexture2D* AmmoIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Properties")
	UTexture2D* PickupIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Properties")
	UTexture2D* CrosshairIcon;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon Properties")
	bool bMagMoving;

	// Functions

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateCrosshair();
};
