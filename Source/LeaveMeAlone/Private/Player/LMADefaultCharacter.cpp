// LeaveMeAlone Game by Netologiya. All Rights Reserved.

#include "Player/LMADefaultCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/InputComponent.h"
#include "Components/LMAHealthComponent.h"
#include "Components/LMAStaminaComponent.h"
#include "Components/LMAWeaponComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

ALMADefaultCharacter::ALMADefaultCharacter()
{

	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArmComponent->SetupAttachment(GetRootComponent());
	SpringArmComponent->SetUsingAbsoluteRotation(true);
	SpringArmComponent->TargetArmLength = ArmLength;
	SpringArmComponent->SetRelativeRotation(FRotator(YRotation, 0.0f, 0.0f));
	SpringArmComponent->bDoCollisionTest = false;
	SpringArmComponent->bEnableCameraLag = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->SetFieldOfView(FOV);
	CameraComponent->bUsePawnControlRotation = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	WeaponComponent = CreateDefaultSubobject<ULMAWeaponComponent>("Weapon");

	HealthComponent = CreateDefaultSubobject<ULMAHealthComponent>("HealthComponent");
	StaminaComponent = CreateDefaultSubobject<ULMAStaminaComponent>("StaminaComponent");
}

void ALMADefaultCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (CursorMaterial)
	{
		CurrentCursor = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), CursorMaterial, CursorSize, FVector(0));
	}

	OnHealthChanged(HealthComponent->GetHealth());
	OnStaminaChanged(StaminaComponent->GetStamina());
	HealthComponent->OnDeath.AddUObject(this, &ALMADefaultCharacter::OnDeath);
	HealthComponent->OnHealthChanged.AddUObject(this, &ALMADefaultCharacter::OnHealthChanged);

	StaminaComponent->OnStaminaChanged.AddUObject(this, &ALMADefaultCharacter::OnStaminaChanged);
}

void ALMADefaultCharacter::Tick(float DeltaTime)
{

	Super::Tick(DeltaTime);
	if (!(HealthComponent->IsDead()))
	{
		RotationPlayerOnCursor();
		SprintProcess(DeltaTime);
	}

}

void ALMADefaultCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ALMADefaultCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ALMADefaultCharacter::MoveRight);
	PlayerInputComponent->BindAxis("CameraZoom", this, &ALMADefaultCharacter::CameraZoom);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ALMADefaultCharacter::BeginSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ALMADefaultCharacter::EndSprint);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, WeaponComponent, &ULMAWeaponComponent::Fire);
	PlayerInputComponent->BindAction("Fire", IE_Released, WeaponComponent, &ULMAWeaponComponent::StopFire);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, WeaponComponent, &ULMAWeaponComponent::Reload);
}

void ALMADefaultCharacter::MoveForward(float Value)
{
	FVector ForwardDirection = FVector::ForwardVector; // Глобальное направление вперед
	AddMovementInput(ForwardDirection, Value);

	//AddMovementInput(GetActorForwardVector(), Value);
}

void ALMADefaultCharacter::MoveRight(float Value)
{
	FVector RightDirection = FVector::RightVector; // Глобальное направление вправо
	AddMovementInput(RightDirection, Value);

	//AddMovementInput(GetActorRightVector(), Value);
}

void ALMADefaultCharacter::CameraZoom(float Value) {

	ArmLength = SetArm(ArmLength + Value);
	SpringArmComponent->TargetArmLength = ArmLength;
}

void ALMADefaultCharacter::BeginSprint() {
	if (!StaminaComponent->StaminaEnd()) {
		IsSprinting = true;
		GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	}
	
}

void ALMADefaultCharacter::EndSprint() {
	IsSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
}

float ALMADefaultCharacter::SetArm(float Value) {
	Value = Value > MaxArm ? MaxArm : Value;
	Value = Value < MinArm ? MinArm : Value;
	return Value;
}

void ALMADefaultCharacter::SprintProcess(float DeltaTime)
{
	if (IsSprinting) {
		StaminaComponent->StaminaDecrease(DeltaTime);
		if (StaminaComponent->StaminaEnd())
		{
			//EndSprint();
			GetCharacterMovement()->MaxWalkSpeed = 300.0f;
		}
	}
	else {
		if (!StaminaComponent->StaminaIsFull())
		{
			StaminaComponent->StaminaIncrease(DeltaTime);
		}
		
	}
}

void ALMADefaultCharacter::OnDeath()
{
	CurrentCursor->DestroyRenderState_Concurrent();

	PlayAnimMontage(DeathMontage);

	GetCharacterMovement()->DisableMovement();

	SetLifeSpan(5.0f);

	if (Controller)
	{
		Controller->ChangeState(NAME_Spectating);
	}
}

void ALMADefaultCharacter::OnHealthChanged(float NewHealth)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Health = %f"), NewHealth));
}

void ALMADefaultCharacter::OnStaminaChanged(float NewStamina)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT("Stamina = %f"), NewStamina));
}

void ALMADefaultCharacter::RotationPlayerOnCursor()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		FHitResult ResultHit;
		PC->GetHitResultUnderCursor(ECC_GameTraceChannel1, true, ResultHit);
		float FindRotatorResultYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ResultHit.Location).Yaw;
		SetActorRotation(FQuat(FRotator(0.0f, FindRotatorResultYaw, 0.0f)));
		if (CurrentCursor)
		{
			CurrentCursor->SetWorldLocation(ResultHit.Location);
		}
	}
}