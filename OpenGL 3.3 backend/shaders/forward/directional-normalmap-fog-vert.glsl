#version 330

layout(location = 0) in vec3 positionMdl;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normalMdl;
layout(location = 3) in vec3 tangentMdl;
layout(location = 4) in vec3 bitangentMdl;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat3 rotation;

uniform vec3 lightDirection;

out vec2 passUv;
out vec3 passLightDirectionTang;
out vec4 passVertexPositionCam;

void main()
{
	passUv = uv;
	
	vec3 rotatedNormal = normalize(rotation * normalMdl);
	
	mat3 modelView3 = mat3(view * model);
	
	vec3 tangentCam = modelView3 * tangentMdl;
	vec3 bitangentCam = modelView3 * bitangentMdl;
	vec3 normalCam = modelView3 * normalMdl; //Normal must have the same rotation as normalmap
	
	//For model space to tangent space transformations
	mat3 tbnInverted = transpose(mat3(
		tangentCam,
		bitangentCam,
		normalCam));
	passLightDirectionTang = tbnInverted * mat3(view) * lightDirection;
	
	vec4 vertexPositionCam = view * model * vec4(positionMdl, 1.0);
	passVertexPositionCam = vertexPositionCam;
	
	gl_Position = projection * view * model * vec4(positionMdl, 1.0);
}