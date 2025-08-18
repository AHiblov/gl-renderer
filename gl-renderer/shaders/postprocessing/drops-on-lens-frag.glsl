#version 450

in vec2 passUv;

out vec3 colour;

uniform sampler2D renderedTexture;
uniform float screenRatio;

const float cellAmount = 10.;
const float cellContentSide = 0.75;
const float cellContentHalfSide = cellContentSide * 0.5;
const float dropSize = 0.12;
const float visibilityMultiplier = 20.;

void main()
{
	vec2 transformedUv = passUv - 0.5; // [-0.5, 0.5]
	transformedUv.x *= screenRatio;
	
	vec2 cell = transformedUv * cellAmount;
	vec2 id = floor(cell);
	
	cell.x += clamp(fract(sin(id.y * 854.78) * 768.83), 0.0, cellContentSide) - cellContentHalfSide;
	cell.y += clamp(fract(sin(id.x * 563.58) * 842.47), 0.0, cellContentSide) - cellContentHalfSide;
	
	cell = fract(cell) - 0.5; //(0; 0) is at center of the cell
	
	float dist = length(cell);
	float mask = smoothstep(dropSize, 0.0, dist);
	
	vec2 rainDistortion = vec2(mask * cell * visibilityMultiplier);
	
	colour = texture(renderedTexture, passUv + rainDistortion).rgb;
}