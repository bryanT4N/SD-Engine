//------------------------------------------------------------------------------------------------
// NetChess3D Blinn-Phong (lit) shader -- REQ-017 production
// Single fixed directional sunlight {3, 1, -2} normalized; ambient floor 0.2;
// pixel shader renormalizes interpolated world normal.
// Compiles against Vertex_PCUTBN (POSITION/COLOR/TEXCOORD/TANGENT/BITANGENT/NORMAL).
//------------------------------------------------------------------------------------------------
struct vs_input_t
{
	float3 modelPosition : POSITION;
	float4 color : COLOR;
	float2 uv : TEXCOORD;
	float3 modelTangent : TANGENT;
	float3 modelBitangent : BITANGENT;
	float3 modelNormal : NORMAL;
};

//------------------------------------------------------------------------------------------------
struct v2p_t
{
	float4 clipPosition : SV_Position;
	float4 color : COLOR;
	float2 uv : TEXCOORD;
	float3 worldTangent : WORLD_TANGENT;
	float3 worldBitangent : WORLD_BITANGENT;
	float3 worldNormal : WORLD_NORMAL;
	float3 modelTangent : MODEL_TANGENT;
	float3 modelBitangent : MODEL_BITANGENT;
	float3 modelNormal : MODEL_NORMAL;
};

//------------------------------------------------------------------------------------------------
cbuffer PerFrameCBOonGPU : register(b1)
{
	float c_time;
	int c_debugInt;
	float c_debugFloat;
	float EMPTY_PADDING;
};

//------------------------------------------------------------------------------------------------
cbuffer CameraCBOonGPU : register(b2)
{
	float4x4 WorldToCameraTransform;
	float4x4 CameraToRenderTransform;
	float4x4 RenderToClipTransform;
};

//------------------------------------------------------------------------------------------------
cbuffer ModelCBOonGPU : register(b3)
{
	float4x4 ModelToWorldTransform;
	float4 ModelColor;
};

//------------------------------------------------------------------------------------------------
Texture2D diffuseTexture : register(t0);
SamplerState diffuseSampler : register(s0);
Texture2D normalTexture : register(t1);
SamplerState normalSampler : register(s1);

//------------------------------------------------------------------------------------------------
v2p_t VertexMain(vs_input_t input)
{
	float4 modelPosition = float4(input.modelPosition, 1.0);
	float4 worldPosition = mul(ModelToWorldTransform, modelPosition);
	float4 cameraPosition = mul(WorldToCameraTransform, worldPosition);
	float4 renderPosition = mul(CameraToRenderTransform, cameraPosition);
	float4 clipPosition = mul(RenderToClipTransform, renderPosition);

	float4 worldTangent = mul(ModelToWorldTransform, float4(input.modelTangent, 0.0));
	float4 worldBitangent = mul(ModelToWorldTransform, float4(input.modelBitangent, 0.0));
	float4 worldNormal = mul(ModelToWorldTransform, float4(input.modelNormal, 0.0));

	v2p_t v2p;
	v2p.clipPosition = clipPosition;
	v2p.color = input.color;
	v2p.uv = input.uv;
	v2p.worldTangent = worldTangent.xyz;
	v2p.worldBitangent = worldBitangent.xyz;
	v2p.worldNormal = worldNormal.xyz;
	v2p.modelTangent = input.modelTangent;
	v2p.modelBitangent = input.modelBitangent;
	v2p.modelNormal = input.modelNormal;
	return v2p;
}

//------------------------------------------------------------------------------------------------
float3 EncodeXYZToRGB(float3 vec)
{
	return (vec + 1.0) * 0.5;
}

//------------------------------------------------------------------------------------------------
float3 DecodeRGBToXYZ(float3 color)
{
	return (color * 2.0) - 1.0;
}

//------------------------------------------------------------------------------------------------
float4 PixelMain(v2p_t input) : SV_Target0
{
	float2 uvCoords = input.uv;
	float4 diffuseTexel = diffuseTexture.Sample(diffuseSampler, uvCoords);
	float4 normalTexel = normalTexture.Sample(normalSampler, uvCoords);
	float4 surfaceColor = input.color;
	float4 modelColor = ModelColor;
	float4 diffuseColor = diffuseTexel * surfaceColor * modelColor;

	float3 lightDir = normalize(float3(3.0, 1.0, -2.0));
	float3 sunlightColor = float3(1.0, 1.0, 1.0);
	float3 worldNormal = normalize(input.worldNormal);
	float diffuseLightDot = dot(-lightDir, worldNormal);
	float lightStrength = clamp(diffuseLightDot, 0.2, 1.0);

	float4 finalColor = float4(diffuseColor.rgb * sunlightColor * lightStrength, diffuseColor.a);
	clip(finalColor.a - 0.01);

//	finalColor.rgb = float3(uvCoords.xy, 0.0);
//	finalColor.rgb = EncodeXYZToRGB(normalize(input.worldTangent));
//	finalColor.rgb = EncodeXYZToRGB(normalize(input.worldBitangent));
//	finalColor.rgb = EncodeXYZToRGB(worldNormal);

	// temporary world-normal TBN check
	finalColor.rgb = EncodeXYZToRGB(worldNormal);
	return finalColor;
}
