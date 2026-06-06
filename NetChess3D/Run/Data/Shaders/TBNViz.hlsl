// TBNViz scaffold
//------------------------------------------------------------------------------------------------
struct vs_input_t
{
	float3 modelSpacePosition : POSITION;
	float4 color : COLOR;
	float2 uv : TEXCOORD;
	float3 modelTangent : TANGENT;
	float3 modelBitangent : BITANGENT;
	float3 modelNormal : NORMAL;
};

//------------------------------------------------------------------------------------------------
struct v2p_t
{
	float4 clipSpacePosition : SV_Position;
	float4 color : COLOR;
	float2 uv : TEXCOORD;
	float3 worldTangent : WORLD_TANGENT;
	float3 worldBitangent : WORLD_BITANGENT;
	float3 worldNormal : WORLD_NORMAL;
};

//------------------------------------------------------------------------------------------------
cbuffer CameraConstantsOnGPU : register(b2)
{
	float4x4 WorldToCameraTransform;
	float4x4 CameraToRenderTransform;
	float4x4 RenderToClipTransform;
};

//------------------------------------------------------------------------------------------------
cbuffer ModelConstantsOnGPU : register(b3)
{
	float4x4 ModelToWorldTransform;
	float4 ModelColor;
};

//------------------------------------------------------------------------------------------------
v2p_t VertexMain(vs_input_t input)
{
	float4 modelSpacePosition = float4(input.modelSpacePosition, 1);
	float4 worldSpacePosition = mul(ModelToWorldTransform, modelSpacePosition);
	float4 cameraSpacePosition = mul(WorldToCameraTransform, worldSpacePosition);
	float4 renderSpacePosition = mul(CameraToRenderTransform, cameraSpacePosition);
	float4 clipSpacePosition = mul(RenderToClipTransform, renderSpacePosition);

	float4 worldTangent = mul(ModelToWorldTransform, float4(input.modelTangent, 0));
	float4 worldBitangent = mul(ModelToWorldTransform, float4(input.modelBitangent, 0));
	float4 worldNormal = mul(ModelToWorldTransform, float4(input.modelNormal, 0));

	v2p_t v2p;
	v2p.clipSpacePosition = clipSpacePosition;
	v2p.color = input.color;
	v2p.uv = input.uv;
	v2p.worldTangent = worldTangent.xyz;
	v2p.worldBitangent = worldBitangent.xyz;
	v2p.worldNormal = worldNormal.xyz;
	return v2p;
}

//------------------------------------------------------------------------------------------------
float4 PixelMain(v2p_t input) : SV_Target0
{
	float3 vizColor = (normalize(input.worldNormal) + 1.0) * 0.5;
//	float3 vizColor = (normalize(input.worldTangent) + 1.0) * 0.5;
//	float3 vizColor = (normalize(input.worldBitangent) + 1.0) * 0.5;
//	float3 vizColor = float3(input.uv.xy, 0);

	return float4(vizColor, 1.0);
}
