//
// Ashikhmin-Shirley fragment shader
//

varying	vec3 l;
varying	vec3 h;
varying vec3 v;
varying	vec3 t;
varying vec3 b;
varying	vec3 n;


uniform vec4 diffColor;
uniform vec4 specColor;

void main (void)
{
        const float PI        = 3.1415926;
        const float	specPower = 30.0;

        float mx = 128.0, my = 128.0;
        float ks = 0.5, kd = 1.0;
        float r0 = 0.5;
        float A = sqrt ( (mx + 1) * (my + 1) / (8 * PI));

        vec3	n2   = normalize ( n );
        vec3	t2   = normalize ( t );
        vec3	b2   = normalize ( b );
        vec3	l2   = normalize ( l );
        vec3	h2   = normalize ( h );
        vec3	v2   = normalize ( v );

        float	nv  = max ( 0.0, dot ( n2, v2 ) );
        float	nl  = max ( 0.0, dot ( n2, l2 ) );
        float	nh  = max ( 0.0, dot ( n2, h2 ) );
        float	hl  = max ( 0.0, dot ( h2, l2 ) );
        float	t1h = dot ( b2, h );
        float	t2h = dot ( t2, h );

                                                // calculate diffuse
        float	rd = (28.0/(23.0*PI)) * ( 1.0 - pow ( 1.0 - 0.5*nv, 5.0 ) ) * ( 1.0 - pow (1.0 - 0.5*nl, 5.0) );

                                                // calculate specular
        float	B  = pow ( nh, (mx*t1h*t1h + my*t2h*t2h)/(1.0 - nh*nh) );
        float	F  = ( r0 + (1.0 - r0) * pow ( 1.0 - hl, 5.0 ) ) / ( hl * max ( nv, nl ) );
        float	rs = A * B * F;

        gl_FragColor = nl * ( diffColor * kd * ( 1.0 - ks ) * rd + specColor * ks * rs );
}
