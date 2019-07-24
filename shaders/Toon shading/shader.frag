//
// Toon fragment shader
//

varying	vec3 l;
varying vec3 n;

uniform vec4 diffColor;
uniform vec4 specColor;

void main (void)
{
        vec3	n2   = normalize ( n );
        vec3	l2   = normalize ( l );
        float	diff = 0.2 + max ( dot ( n2, l2 ), 0.0 );
        vec4	clr;

        if ( diff < 0.4 )
                clr = diffColor * 0.3;
        else if ( diff < 0.7 )
                clr = diffColor ;
        else
                clr = diffColor * 1.3;

        gl_FragColor = clr;
}
