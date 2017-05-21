float4x4 WorldViewProjection;
float4x4 World;
float3 DirLightDir = normalize(float3(0, 0.3, -1));
float4 DirLightDiffuse = float4(1, 1, 1, 0);
texture Texture0;
texture Texture1;
texture Texture2;
float TerrainTexType;

struct VS_INPUT
{
	float4 Position : POSITION0;
	float2 TexCoord : TEXCOORD0;
	float3 Normal : NORMAL0;
};

struct VS_TO_PS
{
	float4 Position : POSITION0;
	float2 TexCoord : TEXCOORD0;
	float3 Normal : TEXCOORD1;
};

struct TERRAIN_VS_INPUT
{
	float4 Position : POSITION0;
	float2 TexCoord : TEXCOORD0;
	float2 TextureType : TEXCOORD1;
	float3 Normal : NORMAL0;
};

struct TERRAIN_VS_TO_PS
{
	float4 Position : POSITION0;
	float2 TexCoord : TEXCOORD0;
	float3 Normal : TEXCOORD1;
	float2 TextureType : TEXCOORD2;
};

sampler BaseTex = sampler_state
{
	Texture = (Texture0);
	MinFilter = ANISOTROPIC;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
};

sampler DirtTex = sampler_state
{
	Texture = (Texture1);
	MinFilter = ANISOTROPIC;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
};

sampler RockTex = sampler_state
{
	Texture = (Texture2);
	MinFilter = ANISOTROPIC;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
};

VS_TO_PS vs_main(in VS_INPUT Input)
{
	VS_TO_PS Output;
	Output.Position = mul(Input.Position, WorldViewProjection);
	Output.TexCoord = Input.TexCoord;
	
	//float3 worldPos = mul(Input.Position, World);
	Output.Normal = mul(Input.Normal, World);
	
	return (Output);
}

float4 ps_main(in TERRAIN_VS_TO_PS Input) : COLOR0
{
	float4 Ambient = float4(0.2f, 0.2f, 0.2f, 1.0f);
	float3 nNormal = normalize(Input.Normal);
	
	float4 BaseColor = tex2D(BaseTex, Input.TexCoord);
	float4 TotalAmbient = BaseColor * Ambient;
	
	//calculate directional light
	//float3 DLDNorm = normalize (DirLightDir);
	float4 TotalDiffuse = BaseColor * saturate(dot(DirLightDir, nNormal));


	
	float4 FinalColor = TotalAmbient + TotalDiffuse;
	return FinalColor;
}

TERRAIN_VS_TO_PS terrain_vs(in TERRAIN_VS_INPUT Input)
{
	TERRAIN_VS_TO_PS Output;
	Output.Position = mul(Input.Position, WorldViewProjection);
	Output.TexCoord = Input.TexCoord;
	Output.TextureType = Input.TextureType;
	
	Output.Normal = mul(Input.Normal, World);
	
	return (Output);
}

float4 terrain_ps(in TERRAIN_VS_TO_PS Input) : COLOR0
{
	float4 Ambient = float4(0.2f, 0.2f, 0.2f, 1.0f);
	float3 nNormal = normalize(Input.Normal);
	
	float4 GrassColor = tex2D(DirtTex, Input.TexCoord);
	float4 DirtColor = tex2D(BaseTex, Input.TexCoord);
	
	//check texture type and blend it
	float4 BaseColor = (float4)0;
	BaseColor += Input.TextureType.x * GrassColor;
	BaseColor += (1 - Input.TextureType.x) * DirtColor;
	
	float4 TotalAmbient = BaseColor * Ambient;
	
	//calculate directional light
	//float3 DLDNorm = normalize (DirLightDir);
	float4 TotalDiffuse = BaseColor * saturate(dot(DirLightDir, nNormal));


	
	float4 FinalColor = TotalAmbient + TotalDiffuse;
	return FinalColor;
}

technique vpshader
{
	pass pass0
	{
		vertexshader = compile vs_3_0 vs_main();
		pixelshader = compile ps_3_0 ps_main();
	}
}

technique terrain
{
	pass pass0
	{
		vertexshader = compile vs_3_0 terrain_vs();
		pixelshader = compile ps_3_0 terrain_ps();
	}
}

technique noshader
{
	pass pass0
	{
		vertexshader = null;
		pixelshader = null;
	}
}