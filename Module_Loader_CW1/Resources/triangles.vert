#version 400 core

layout( location = 0 ) in vec3 vPosition;
layout( location = 1 ) in vec4 vColour;
layout( location = 2 ) in vec3 vNormal;
layout( location = 3 ) in vec2 aTexCoord;

uniform vec3 lightPos;
uniform vec4 ambient;
uniform vec3 dLight;

uniform vec3 sLight;
uniform float sShine;

uniform mat4 mvMatrix;
uniform mat4 pMatrix;

out vec4 fragColour;
out vec2 TexCoord;


void main()
{
    vec4 P = mvMatrix * vec4(vPosition,1.0);
	vec3 N = mat3(mvMatrix) * vNormal;
	vec3 L = lightPos - P.xyz;
	vec3 V = -P.xyz;
	
	N = normalize(N);
	L = normalize(L);
	V = normalize(V);

	vec3 R = reflect(-L,N);	
	vec3 diffuse = max(0.0, dot(N,L)) * dLight;
	vec3 specular = pow(max(dot(R,V), 0.0), sShine) * sLight;
	fragColour =  (ambient + vec4(diffuse,1.0) + vec4(specular,1.0) ) * vColour;

	gl_Position = pMatrix * P;
	TexCoord = aTexCoord;
}
