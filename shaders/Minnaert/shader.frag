//
// Minnaert lighting model fragment shader
//

varying	vec3 l;
varying	vec3 h;
varying vec3 v;
varying vec3 n;

uniform vec4 diffColor;

uniform float k;

void main (void)
{
        vec3	n2 = normalize ( n );
        vec3	l2 = normalize ( l );
        vec3	v2 = normalize ( v );
        float	d1 = pow ( max ( dot ( n2, l2 ), 0.0 ), 1.0 + k );
        float	d2 = pow ( 1.0 - dot ( n2, v2 ), 1.0 - k );

        gl_FragColor = diffColor * d1 * d2;
}
