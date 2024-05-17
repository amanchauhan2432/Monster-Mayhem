#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "StellaController.generated.h"

UCLASS()
class MONSTER_MAYHEM_API AStellaController : public APlayerController
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<class UUserWidget> WidgetClass;

	UUserWidget* AmmoWidget;

protected:
	virtual void BeginPlay() override;
};
