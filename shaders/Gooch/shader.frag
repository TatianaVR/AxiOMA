//
// Fragment shader for Gooch shading
//
// Based on 3Dlabs code by Randi Rost
//

uniform vec3  diffColor;

varying float NdotL;
varying vec3  reflectVec;
varying vec3  viewVec;

uniform float diffuseWarm;
uniform float diffuseCool;
uniform float specPower;

void main (void)
{
    const	vec3  warmColor    = vec3 ( 0.6, 0.6, 0.0 );
    const	vec3  coolColor    = vec3 ( 0.0, 0.0, 0.6 );

    vec3 kCool    = min ( coolColor + diffuseCool * diffColor, 1.0 );
    vec3 kWarm    = min ( warmColor + diffuseWarm * diffColor, 1.0 );
    vec3 kFinal   = mix ( kCool, kWarm, NdotL );

    vec3  r    = normalize ( reflectVec );
    vec3  v    = normalize ( viewVec    );
    float spec = pow ( max ( dot ( r, v ), 0.0 ), specPower );

    gl_FragColor = vec4 ( min ( kFinal + spec, 1.0 ), 1.0 );
}
