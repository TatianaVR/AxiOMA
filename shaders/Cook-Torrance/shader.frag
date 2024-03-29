//
// Cook-Torrance fragment shader
//

varying	vec3 l;
varying	vec3 h;
varying vec3 v;
varying vec3 n;


uniform vec4 diffColor;
uniform vec4 specColor;

uniform float r0;
uniform float roughness;

void main (void)
{
        const float	e         = 2.7182818284;
        const float pi        = 3.1415926;

        vec3	n2   = normalize ( n );
        vec3	l2   = normalize ( l );
        vec3	v2   = normalize ( v );
        vec3	h2   = normalize ( h );
        float	nh   = dot ( n2, h2 );
        float	nv   = dot ( n2, v2 );
        float	nl   = dot ( n2, l2 );


        float	r2   = roughness * roughness;
        float	nh2  = nh * nh;
        float   ex   = - (1.0 - nh2)/(nh2 * r2);
        float	d    = pow ( e, ex ) / (r2*nh2*nh2);

        float	f    = mix ( pow ( 1.0 - nv, 5.0 ), 1.0, r0 );		// Fresnel
        float	x    = 2.0 * nh / dot ( v2, h2 );
        float	g    = min ( 1.0, min ( x * nl, x * nv ) );			// Geometry attenuation
        float	ct   = d*f*g / nv;

        vec4	diff = diffColor * max ( 0.0, nl );
        vec4	spec = specColor * max ( 0.0, ct );

        gl_FragColor = diff + spec;
}
