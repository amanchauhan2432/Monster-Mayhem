#include "Explosive.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../Character/Stella.h"
#include "../Enemies/Enemy.h"
#include "../Enemies/EnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"

AExplosive::AExplosive()
{
	PrimaryActorTick.bCanEverTick = false;

	ExplosiveMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Explosive Mesh"));
	SetRootComponent(ExplosiveMesh);
	ExplosiveMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collision"));
	SphereCollision->SetupAttachment(ExplosiveMesh);
	SphereCollision->SetSphereRadius(250.f);
}

void AExplosive::BulletHit_Implementation(FVector HitLocation)
{
	if (HitEffect && HitSound)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEffect, HitLocation);

		UGameplayStatics::SpawnSoundAtLocation(this, HitSound, HitLocation);

		TArray<AActor*> ActorsInRange;
		GetOverlappingActors(ActorsInRange, ACharacter::StaticClass());
		if (ActorsInRange.Num() > 0)
		{
			for (auto Actor : ActorsInRange)
			{
				UGameplayStatics::ApplyDamage(Actor, BaseDamage, GetWorld()->GetFirstPlayerController(), this, UDamageType::StaticClass());

				if (auto Enemy = Cast<AEnemy>(Actor))
				{
					if (auto EnemyController = Cast<AEnemyController>(Enemy->GetController()))
					{
						if (auto Player = Cast<AStella>(UGameplayStatics::GetPlayerCharacter(this, 0)))
						{
							EnemyController->BlackboardComponent->SetValueAsObject(FName("Target"), Player);
						}
					}
				}
			}
		}

		Destroy();
	}
}
