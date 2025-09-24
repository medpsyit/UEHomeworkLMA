//  LeaveMeAlone Game by Netologiya. All RightsReserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LMAStaminaComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnStaminaChanged, float);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LEAVEMEALONE_API ULMAStaminaComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULMAStaminaComponent();

	UFUNCTION(BlueprintCallable)
	float GetStamina() const { return Stamina; }

	FOnStaminaChanged OnStaminaChanged;

private:

	float Stamina = 0.0f;
	float StaminaCost = 40.0f;
	float StaminaRegenRate = 50.0f; 

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float MaxStamina = 100.0f;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void StaminaIncrease(float DeltaTime);
	void StaminaDecrease(float DeltaTime);
	bool StaminaIsFull();
	bool StaminaEnd();
		
};
