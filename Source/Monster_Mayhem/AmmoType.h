#pragma once

UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	EAT_9mm UMETA(DisplayName = "9mm"),
	EAT_Rifle UMETA(DisplayName = "Rifle"),

	EAT_MAX UMETA(DisplayName = "DefaultMAX")
};