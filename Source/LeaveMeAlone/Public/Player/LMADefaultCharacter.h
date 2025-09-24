// LeaveMeAlone Game by Netologiya. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LMADefaultCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class ULMAWeaponComponent;
class ULMAHealthComponent;
class ULMAStaminaComponent;
class UAnimMontage;

UCLASS()
class LEAVEMEALONE_API ALMADefaultCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ALMADefaultCharacter();

	UFUNCTION()
	ULMAHealthComponent* GetHealthComponent() const { return HealthComponent; }  

	UFUNCTION()
	ULMAStaminaComponent* GetStaminaComponent() const { return StaminaComponent; }  

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	ULMAWeaponComponent* WeaponComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components|Health")
	ULMAHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components|Stamina")
	ULMAStaminaComponent* StaminaComponent;

	UPROPERTY()
	UDecalComponent* CurrentCursor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
	UMaterialInterface* CursorMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
	FVector CursorSize = FVector(20.0f, 40.0f, 40.0f);

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arm")
	float MinArm = 300.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arm")
	float MaxArm = 1500.0f;

	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	float YRotation = -75.0f;
	float ArmLength = 1400.0f;
	float FOV = 55.0f;

	bool IsSprinting = false;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void CameraZoom(float Value);
	void BeginSprint();
	void EndSprint();

	float SetArm(float Value);
	void SprintProcess(float DeltaTime);

	void OnDeath();
	void OnHealthChanged(float NewHealth);

	void OnStaminaChanged(float NewStamina);

	void RotationPlayerOnCursor();
};