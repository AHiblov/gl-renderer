#version 450

uniform sampler2D colourTexture;
uniform vec3 lightPositionWld;

in vec2 passUv;
in vec3 passPositionWld;
in vec3 passLightDirectionCam;
in vec3 passNormalCam;
in vec3 passNormalMdl;

const float cellAmount = 4.f;
const float triplanarMappingSharpness = 5.0f;

out vec4 colour;

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
	
	vec3 curColour = vec3(0.f);
	
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
	float lightPower = 50.0;
	
	vec4 diffuseColor = vec4(performTriplanarMapping(passPositionWld, passNormalMdl, triplanarMappingSharpness), 1.0);
	vec4 ambientColor = diffuseColor * vec4(0.25, 0.25, 0.25, 1.0);
	
	float distanceToLight = length(lightPositionWld - passPositionWld);
	
	float cosTheta = clamp(dot(passNormalCam, passLightDirectionCam), 0.0, 1.0);
	
	colour = vec4(ambientColor + 
		diffuseColor * lightPower * cosTheta / (distanceToLight * distanceToLight));
}