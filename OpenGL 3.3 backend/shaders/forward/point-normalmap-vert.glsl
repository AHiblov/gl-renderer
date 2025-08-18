#version 330

layout(location = 0) in vec3 positionMdl;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normalMdl;
layout(location = 3) in vec3 tangentMdl;
layout(location = 4) in vec3 bitangentMdl;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPositionWld;

out vec2 passUv;
out vec3 passPositionWld;
out vec3 passLightDirectionTang;

void main()
{
	vec4 positionCam = view * model * vec4(positionMdl, 1.0);

	gl_Position = projection * positionCam;
   
	passUv = uv;
   
	passPositionWld = (model * vec4(positionMdl, 1.0)).xyz;
   
	//Vector that goes from the vertex to the camera, in camera space
	vec3 vertexCam = positionCam.xyz;
	vec3 eyeDirectionCam = vec3(0.0, 0.0, 0.0) - vertexCam;
   
	//Vector that goes from the vertex to the light, in camera space
	vec3 lightPositionCam = (view * vec4(lightPositionWld, 1.0)).xyz;
	vec3 lightDirectionCam = normalize(lightPositionCam + eyeDirectionCam);
   
	mat3 modelView3 = mat3(view * model);
	
	vec3 tangentCam = modelView3 * tangentMdl;
	vec3 bitangentCam = modelView3 * bitangentMdl;
	vec3 normalCam = modelView3 * normalMdl;
	
	//For model space to tangent space transformations
	mat3 tbnInverted = transpose(mat3(
		tangentCam,
		bitangentCam,
		normalCam));
	
	passLightDirectionTang = tbnInverted * lightDirectionCam;
}
