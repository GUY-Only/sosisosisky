// We Sir

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerDeathHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOSISOSISKY_API UPlayerDeathHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public: 
    // Привяжется к анимации из UMG с таким же именем
    UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* FadeInAnimation;

    UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* FadeOutAnimation;
};
