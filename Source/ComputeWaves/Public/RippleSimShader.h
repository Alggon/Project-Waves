#pragma once

#include "CoreMinimal.h"
#include "GlobalShader.h"
#include "ShaderParameterStruct.h"
#include "RenderGraphResources.h"

/**
 * 
 */
class COMPUTEWAVES_API FRippleSimCS : public FGlobalShader
{
public:

	DECLARE_GLOBAL_SHADER(FRippleSimCS);
	SHADER_USE_PARAMETER_STRUCT(FRippleSimCS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_RDG_TEXTURE(Texture2D, PrevBuffer)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float2>, NextBuffer)
		SHADER_PARAMETER(int32, TextureRes)
		SHADER_PARAMETER(float, DeltaTime)
		SHADER_PARAMETER(float, Damping)
		SHADER_PARAMETER(float, WaveSpeed)
		SHADER_PARAMETER(FVector2f, DropPos)
		SHADER_PARAMETER(float, DropRadius)
		SHADER_PARAMETER(float, DropStrength)
		SHADER_PARAMETER(float, DropAge)
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return true;
	}
};
