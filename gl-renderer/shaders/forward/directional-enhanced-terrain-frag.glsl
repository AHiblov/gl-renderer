#version 450

uniform sampler2D colourTexture;
uniform vec3 lightDirection;
uniform vec3 diffuseLightColour;
uniform vec3 ambientLightColour;

in vec3 passPosition;
in vec2 passUv;
in vec3 passNormal;

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

vec4 hash4(vec2 p)
{
	return fract(sin(vec4(1.0 + dot(p, vec2(37.0,17.0)), 
						  2.0 + dot(p, vec2(11.0,47.0)),
						  3.0 + dot(p, vec2(41.0,29.0)),
						  4.0 + dot(p, vec2(23.0,31.0)))) * 103.0);
}

//Variant 2. https://www.shadertoy.com/view/4tsGzf (modified)
/*vec3 performTextureBombing(vec2 uv)
{
	vec2 cellId = floor( uv );
	vec2 cellFractional = fract( uv );
	
	// derivatives (for correct mipmapping)
	vec2 ddx = dFdx( uv );
	vec2 ddy = dFdy( uv );
    
	vec3 finalColour = vec3(0.0);
	float linearWeight = 0.0;
	for(int i = -1; i <= 1; i++)
		for(int j = -1; j <= 1; j++)
		{
			vec2 curCell = vec2(float(i), float(j));
			vec4 randOffset = hash4(cellId + curCell);
			vec2 r = curCell - cellFractional + randOffset.xy;
			float d = dot(r, r);
			float weight = exp(-5.0 * d);
			vec3 curColour = textureGrad(colourTexture, uv + randOffset.zw, ddx, ddy).xyz;
			finalColour += weight * curColour;
			linearWeight += weight;
		}

	return finalColour / linearWeight;
}*/

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
	//vec4 textureDiffuseColour = vec4(performTextureBombing(passUv /** cellAmount*/), 1.0); //Multiplication by cellAmount is only for the variant 2
	vec4 textureDiffuseColour = vec4(performTriplanarMapping(passPosition, passNormal, triplanarMappingSharpness), 1.0);
	vec4 textureAmbientColour = textureDiffuseColour * vec4(0.25, 0.25, 0.25, 1.0);
	
	colour = textureAmbientColour * vec4(ambientLightColour, 1.0) + textureDiffuseColour * clamp(dot(passNormal, -lightDirection), 0.0, 1.0) * vec4(diffuseLightColour, 1.0);
}