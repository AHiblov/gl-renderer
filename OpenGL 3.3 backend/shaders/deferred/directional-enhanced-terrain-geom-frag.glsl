#version 330

uniform sampler2D colourTexture;

in vec3 passPosition;
in vec2 passUv;
in vec3 passNormal;

const float cellAmount = 4.f;
const float triplanarMappingSharpness = 5.0f;

layout(location = 0) out vec3 positionWld;
layout(location = 1) out vec3 uv;
layout(location = 2) out vec3 normal;
layout(location = 3) out vec3 diffuse;

//https://www.shadertoy.com/view/4djSRW
vec2 hash22(vec2 cell)
{
	vec3 p3 = fract(vec3(cell.xyx) * vec3(.1031, .1030, .0973));
	p3 += dot(p3, p3.yzx + 33.33);
	return fract((p3.xx + p3.yz) * p3.zy);
}

//Variant 1. GPU Gems
vec3 performTextureBombing(vec2 uv)
{
	vec2 scaledUV = uv * cellAmount;
	vec2 cell = floor(scaledUV);
	vec2 offset = scaledUV - cell;
	
	vec3 curColour = vec3(0.0);
	
	for(int i = -1; i <= 0; i++)
		for(int j = -1; j <= 0; j++)
		{
			vec2 srcCell = cell + vec2(i, j);
			vec2 randomInput = hash22(srcCell);
            
			vec2 curOffset = offset - vec2(i, j);
            
			curColour += texture(colourTexture, curOffset - randomInput).rgb * 0.25;
		}

	return curColour;
}

//https://www.shadertoy.com/view/MtsGWH (modified)
vec3 performTriplanarMapping(vec3 point, vec3 normal, float sharpness)
{
	//Without Texture Bombing
	/*vec3 x = texture(colourTexture, point.yz).rgb;
	vec3 y = texture(colourTexture, point.zx).rgb;
	vec3 z = texture(colourTexture, point.xy).rgb;*/
	
	vec3 x = performTextureBombing(point.yz).rgb;
	vec3 y = performTextureBombing(point.zx).rgb;
	vec3 z = performTextureBombing(point.xy).rgb;
	
	vec3 mixFactor = pow(abs(normal), vec3(sharpness));
	
	return (x * mixFactor.x + y * mixFactor.y + z * mixFactor.z) / (mixFactor.x + mixFactor.y + mixFactor.z);
}

void main()
{
	positionWld = passPosition;
	uv = vec3(passUv, 0.0);
	normal = passNormal;
	
	diffuse = performTriplanarMapping(passPosition, passNormal, triplanarMappingSharpness);
}