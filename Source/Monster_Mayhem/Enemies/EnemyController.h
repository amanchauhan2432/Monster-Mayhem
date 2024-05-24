#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyController.generated.h"

UCLASS()
class MONSTER_MAYHEM_API AEnemyController : public AAIController
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	class UBehaviorTreeComponent* BehaviorTreeComponent;

	UPROPERTY(EditDefaultsOnly)
	UBlackboardComponent* BlackboardComponent;

protected:

public:
	AEnemyController();

	virtual void OnPossess(APawn* InPawn) override;
};
