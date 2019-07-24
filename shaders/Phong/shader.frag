//
// Phong fragment shader
//

varying vec3 l;
varying vec3 v;
varying vec3 n;

uniform vec4 diffColor;
uniform vec4 specColor;

uniform float specPower;

void main (void)
{
    vec3 n2   = normalize ( n );
    vec3 l2   = normalize ( l );
    vec3 v2   = normalize ( v );
    vec3 r    = reflect ( -v2, n2 );
    vec4 diff = diffColor * max ( dot ( n2, l2 ), 0.0 );
    vec4 spec = specColor * pow ( max ( dot ( l2, r ), 0.0 ), specPower );

    gl_FragColor = diff + spec;
}
