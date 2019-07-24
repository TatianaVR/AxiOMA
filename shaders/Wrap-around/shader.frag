//
// Wrap-around lighting fragment shader
//

varying vec3 l;
varying vec3 n;

uniform vec4 diffColor;
uniform float factor;

void main (void)
{
    vec3 n2   = normalize ( n );
    vec3 l2   = normalize ( l );
    vec4 diff = diffColor * max ( dot ( n2, l2 ) + factor, 0.0 ) / ( 1.0 + factor );

    gl_FragColor = diff;
}
