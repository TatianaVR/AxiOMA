//
// Subsurface scattering vertex shader
//

varying vec3 n, eyeVec, lightVec, vertPos, lightPos;

void main()
{
    const vec3	lightPosition = vec3 ( 5, 0, 4 );
    vec4 pos = gl_ModelViewProjectionMatrix * gl_Vertex;

    n        = gl_NormalMatrix * gl_Normal;
    lightVec = normalize ( lightPosition - pos.xyz );
    vertPos  = pos.xyz;
    lightPos = lightPosition;
    eyeVec   = normalize ( -pos.xyz );

    gl_Position    = pos;
}
