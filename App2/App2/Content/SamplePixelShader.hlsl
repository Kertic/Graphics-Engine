// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 worldPos : POSITION3;
	float3 color : COLOR0;
	float3 normWorld : NORMAL;
	float3 normView : NORMAL2;

	float4 camPosition : POSITION4;

	float3 Lightnorm : NORMAL1;
	float3 Lightcolor : COLOR1;
	float3 Lightpos : POSITION1;
	float3 Lighttype : POSITION2;
	float Specularity : COLOR2;

};

// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{
	//This is directional Light
	 if (input.Lighttype.x >= 0.0f && input.Lighttype.x < 0.8f) {
		 float lightRatio = saturate(dot(-input.Lightnorm, input.normWorld));
		 input.color = lightRatio * input.Lightcolor * input.color;
		}
//point light
if (input.Lighttype.x >= 0.8f && input.Lighttype.x < 1.8f) {
	float3 pos = float3(input.worldPos.xyz);
	float3 lightDir = normalize(input.Lightpos - pos);
	float lightRatio = saturate(dot(lightDir, input.normWorld));
	input.color = lightRatio * input.Lightcolor * input.color;
}
//Spot light
if (input.Lighttype.x >= 1.8f && input.Lighttype.x < 2.8f) {
	float3 pos = float3(input.worldPos.xyz);
	float3 lightDir = normalize(input.Lightpos - pos);
	float spotfactor = 0.0f;
	float surfaceRatio = saturate(dot(-lightDir, input.Lightnorm));
	//0.5 is the cone ratio
	if (surfaceRatio > 0.7) {
		spotfactor = 1.0f;
	}
	float lightRatio = saturate(dot(lightDir, input.normWorld));
	input.color = lightRatio * spotfactor * input.color * input.Lightcolor;
}
float3 viewDir = normalize(float3(input.camPosition.xyz) - float3(input.worldPos.xyz));
float3 halfVector = normalize((-input.Lightnorm) - viewDir);
float intensity = 0.0f;
float testValue = pow(saturate(dot(input.normView, halfVector)), input.Specularity);
if (testValue > intensity) {
	intensity = testValue;
}
float3 reflectedLight = input.Lightcolor * intensity;



input.color = saturate(input.color + reflectedLight);

	return float4(input.color, 1.0f);
}
