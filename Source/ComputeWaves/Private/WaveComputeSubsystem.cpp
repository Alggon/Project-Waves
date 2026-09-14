// Fill out your copyright notice in the Description page of Project Settings.


#include "WaveComputeSubsystem.h"
#include "WaveComputeShader.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "GlobalShader.h"
#include "RenderTargetPool.h"
#include "Kismet/KismetRenderingLibrary.h"


void UWaveComputeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	OutputRenderTarget = Cast<UTextureRenderTarget2D>(GetDefault<UWaveComputingSettings>()->OutputRenderTarget.TryLoad());
	OutputNormal = Cast<UTextureRenderTarget2D>(GetDefault<UWaveComputingSettings>()->OutputNormal.TryLoad());
	CachedResolution = GetDefault<UWaveComputingSettings>()->CachedResolution;
	CachedWorldSize = GetDefault<UWaveComputingSettings>()->CachedWorldSize;
	FoamThreshold = GetDefault<UWaveComputingSettings>()->FoamThreshold;
	FoamDecay = GetDefault<UWaveComputingSettings>()->FoamDecay;
	Waves = GetDefault<UWaveComputingSettings>()->Waves;
}

void UWaveComputeSubsystem::Deinitialize()
{
	FlushRenderingCommands();
	HeightReadback.Reset();
	Super::Deinitialize();
}

void UWaveComputeSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!OutputRenderTarget || !OutputRenderTarget->GetResource())
	{
		return;
	}

	if (!HeightReadback.IsValid())
	{
		HeightReadback = MakeUnique<FRHIGPUTextureReadback>(TEXT("FinalHeightReadback"));
	}

	FTextureRenderTargetResource* RTResource = OutputRenderTarget->GameThread_GetRenderTargetResource();
	FTextureRenderTargetResource* RTNormalResource = OutputNormal->GameThread_GetRenderTargetResource();

	const int32 WaveCount = Waves.Num();
	const float CurrentTime = GetWorld()->GetTimeSeconds();
	const float WorldSize = CachedWorldSize;
	const int32 Resolution = CachedResolution;
	const float ConstDeltaTime = GetWorld()->GetDeltaSeconds();

	const float TempFoamThreshold = FoamThreshold;
	const float TempFoamDecay = FoamDecay;

	const TArray<FWaveParams> TempWaves = Waves;

	const bool bShouldRequestCopy = !bHeightReadbackPending;

	FRHIGPUTextureReadback* ReadbackPtr = HeightReadback.Get();

	if (!bIsCurrentlyDispatching)
	{
		bIsCurrentlyDispatching = true;

		if (bShouldRequestCopy)
		{
			bHeightReadbackPending = true;
		}

		ENQUEUE_RENDER_COMMAND(WaveComputeDispatch)(
			[RTResource, RTNormalResource, WaveCount, CurrentTime, WorldSize, Resolution, ConstDeltaTime, bShouldRequestCopy, TempFoamThreshold, TempFoamDecay, TempWaves, ReadbackPtr, this](FRHICommandListImmediate& RHICmdList)
			{
				FRDGBuilder GraphBuilder(RHICmdList);

				FRDGTextureRef ExternalTex = RegisterExternalTexture(GraphBuilder, RTResource->GetRenderTargetTexture(), TEXT("WaveOutputExternal"));
				FRDGTextureRef ExternalNormalTex = RegisterExternalTexture(GraphBuilder, RTNormalResource->GetRenderTargetTexture(), TEXT("WaveOutputExternal"));

				FWaveComputeCS::FParameters* Params = GraphBuilder.AllocParameters<FWaveComputeCS::FParameters>();

				Params->OutputTexture = GraphBuilder.CreateUAV(ExternalTex);
				Params->OutputNormal = GraphBuilder.CreateUAV(ExternalNormalTex);
				Params->WaveCount = WaveCount;
				Params->Time = CurrentTime;
				Params->WorldSize = WorldSize;
				Params->TextureRes = Resolution;
				Params->DeltaTime = ConstDeltaTime;
				Params->FoamThreshold = TempFoamThreshold;
				Params->FoamDecay = TempFoamDecay;

				FRDGBufferRef WaveBuffer = GraphBuilder.CreateBuffer(
					FRDGBufferDesc::CreateStructuredDesc(sizeof(FWaveParams), TempWaves.Num()),
					TEXT("WaveBuffer")
				);

				GraphBuilder.QueueBufferUpload(
					WaveBuffer,
					TempWaves.GetData(),
					TempWaves.Num() * sizeof(FWaveParams)
				);

				Params->Waves = GraphBuilder.CreateSRV(WaveBuffer);

				TShaderMapRef<FWaveComputeCS> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

				FComputeShaderUtils::AddPass(
					GraphBuilder,
					RDG_EVENT_NAME("WaveCompute"),
					ComputeShader,
					Params,
					FIntVector(FMath::DivideAndRoundUp(Resolution, 8), FMath::DivideAndRoundUp(Resolution, 8), 1)
				);

				if (bShouldRequestCopy)
				{
					AddEnqueueCopyPass(GraphBuilder, ReadbackPtr, ExternalTex);
				}

				GraphBuilder.Execute();

				bIsCurrentlyDispatching = false;
			});
	}

	if (bHeightReadbackPending && HeightReadback.IsValid() && HeightReadback->IsReady())
	{
		FRHIGPUTextureReadback* ReadbackPtrForLock = HeightReadback.Get();

		ENQUEUE_RENDER_COMMAND(HeightReadbackLock)(
			[Resolution, ReadbackPtrForLock, this](FRHICommandListImmediate& RHICmdList)
			{
				const uint32 NumPixels = Resolution * Resolution;
				const uint32 BytesPerPixel = sizeof(float) * 4;

				void* RawData = ReadbackPtrForLock->Lock(NumPixels * BytesPerPixel);

				if (RawData)
				{
					const FLinearColor* PixelData = static_cast<const FLinearColor*>(RawData);

					FScopeLock Lock(&HeightCacheLock);
					CachedHeightData.SetNumUninitialized(NumPixels);

					for (int32 Y = 0; Y < Resolution; ++Y)
					{
						for (int32 X = 0; X < Resolution; ++X)
						{
							const uint32 Index = Y * Resolution + X;

							CachedHeightData[Index] = PixelData[Index].B;
						}
					}

					ReadbackPtrForLock->Unlock();
				}
			});


		bHeightReadbackPending = false;
	}
}

TStatId UWaveComputeSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UWaveComputeSubsystem, STATGROUP_Tickables);
}

FVector UWaveComputeSubsystem::GetWaveHeightAtWorldPos(FVector WorldPos) const
{
	FScopeLock Lock(&HeightCacheLock);

	if (CachedHeightData.Num() == 0 || CachedWorldSize <= 0.0f)
	{
		return FVector::ZeroVector;
	}

	FVector2D UV = (FVector2D(WorldPos.X, WorldPos.Y) / CachedWorldSize) + FVector2D(0.5f, 0.5f);
	int32 X = FMath::Clamp(FMath::FloorToInt(UV.X * CachedResolution), 0, CachedResolution - 1);
	int32 Y = FMath::Clamp(FMath::FloorToInt(UV.Y * CachedResolution), 0, CachedResolution - 1);

	return FVector(WorldPos.X, WorldPos.Y, CachedHeightData[Y * CachedResolution + X]);
}
