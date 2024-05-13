#include "StellaAnimInstance.h"
#include "Stella.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UStellaAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	StellaCharacter = Cast<AStella>(TryGetPawnOwner());
}

void UStellaAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (StellaCharacter)
	{
		GroundSpeed = StellaCharacter->GetVelocity().Size2D();
		bIsInAir = StellaCharacter->GetCharacterMovement()->IsFalling();

		FRotator AimRotation = StellaCharacter->GetBaseAimRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(StellaCharacter->GetVelocity());

		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

		bIsAiming = StellaCharacter->bIsAiming;
	}
}