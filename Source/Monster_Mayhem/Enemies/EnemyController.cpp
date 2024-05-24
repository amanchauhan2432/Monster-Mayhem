#include "EnemyController.h"
#include "Enemy.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

AEnemyController::AEnemyController()
{
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("Behavior Tree Component"));

	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard Component"));
}

void AEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AEnemy* Enemy = Cast<AEnemy>(InPawn);
	if (Enemy && Enemy->BehaviorTree)
	{
		BlackboardComponent->InitializeBlackboard(*Enemy->BehaviorTree->BlackboardAsset);
	}
}
