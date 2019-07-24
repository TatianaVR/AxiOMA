//
// Lommel-Seeliger fragment shader
//

varying	vec3 l;
varying	vec3 h;
varying vec3 v;
varying vec3 n;

uniform vec4 diffColor;

void main (void)
{
        vec3	n2   = normalize ( n );
        vec3	l2   = normalize ( l );
        vec3	v2   = normalize ( v );
        float	a    = max ( 0.0, dot       ( n2, l2 ) );
        float	b    = max ( 0.0, dot       ( n2, v2 ) );

        gl_FragColor = diffColor * a / (a + b);
}


