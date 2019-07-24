//
// Lambert fragment shader
//

varying vec3 l;
varying vec3 n;

uniform vec4 diffColor;

void main (void)
{
    vec3 n2   = normalize ( n );
    vec3 l2   = normalize ( l );
    vec4 diff = diffColor * max ( dot ( n2, l2 ), 0.0 );

    gl_FragColor = diff;
}
