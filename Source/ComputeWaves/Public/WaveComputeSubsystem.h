// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Engine/TextureRenderTarget2D.h"
#include "RHIGPUReadback.h"
#include "WaveComputingSettings.h"
#include "WaveComputeSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class COMPUTEWAVES_API UWaveComputeSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public :
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickable() const override { return true; };

	UFUNCTION(BlueprintCallable)
	FVector GetWaveHeightAtWorldPos(FVector WorldPos) const;

private :

	UPROPERTY()
	TObjectPtr<UTextureRenderTarget2D> OutputRenderTarget = nullptr;
	UPROPERTY()
	TObjectPtr<UTextureRenderTarget2D> OutputNormal = nullptr;
	UPROPERTY()
	int32 CachedResolution = 256;
	UPROPERTY()
	float CachedWorldSize = 1000.f;
	UPROPERTY()
	TArray<FWaveParams> Waves;
	UPROPERTY()
	float FoamThreshold = 0.5f;
	UPROPERTY()
	float FoamDecay = 0.8f;

	FThreadSafeBool bIsCurrentlyDispatching = false;

	bool bHeightReadbackPending = false;

	mutable FCriticalSection HeightCacheLock;

	TUniquePtr<FRHIGPUTextureReadback> HeightReadback;

	TArray<float> CachedHeightData;
};
