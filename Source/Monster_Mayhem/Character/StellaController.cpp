#include "StellaController.h"
#include "Blueprint/UserWidget.h"

void AStellaController::BeginPlay()
{
	Super::BeginPlay();

	if (WidgetClass)
	{
		AmmoWidget = CreateWidget<UUserWidget>(this, WidgetClass);
		if (AmmoWidget)
		{
			AmmoWidget->AddToViewport();
		}
	}
}
