// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "WaveComputingSettings.generated.h"

USTRUCT(BlueprintType)
struct FWaveParams
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	FVector2f Direction = FVector2f(1.f,0.f);
	UPROPERTY(EditAnywhere)
	float Amplitude = 50.f;
	UPROPERTY(EditAnywhere)
	float Omega = 0.01f;
	UPROPERTY(EditAnywhere)
	float Phi = 1.f;
	UPROPERTY(EditAnywhere)
	float Q = 0.5f;

	FVector2f Padding = FVector2f(0);
};
/**
 *
 */
UCLASS(Config = Game, DefaultConfig)
class COMPUTEWAVES_API UWaveComputingSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	virtual FName GetCategoryName() const override { return FName("Project"); };

	UPROPERTY(Config, EditAnywhere, Category = "Textures", meta = (AllowedClasses = "/Script/Engine.TextureRenderTarget2D"))
	FSoftObjectPath OutputRenderTarget = nullptr;
	UPROPERTY(Config, EditAnywhere, Category = "Textures", meta = (AllowedClasses = "/Script/Engine.TextureRenderTarget2D"))
	FSoftObjectPath OutputNormal = nullptr;
	UPROPERTY(Config, EditAnywhere, Category = "Textures", meta = (AllowedClasses = "/Script/Engine.MaterialParameterCollection"))
	FSoftObjectPath PostProcessCollection = nullptr;

	UPROPERTY(Config, EditAnywhere, Category = "Foam")
	float FoamThreshold = 0.5f;
	UPROPERTY(Config, EditAnywhere, Category = "Foam")
	float FoamDecay = 0.8f;
	UPROPERTY(Config, EditAnywhere, Category = "Waves")
	TArray<FWaveParams> Waves;
	UPROPERTY(Config, EditAnywhere, Category = "Sizes")
	int32 CachedResolution = 256;
	UPROPERTY(Config, EditAnywhere, Category = "Sizes")
	float CachedWorldSize = 1000.f;
};
