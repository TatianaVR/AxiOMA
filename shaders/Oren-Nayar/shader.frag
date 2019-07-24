//
// Oren-Nayar fragment shader
//

varying	vec3 l;
varying vec3 v;
varying vec3 n;

uniform vec4 diffColor;

uniform float roughness;

void main (void)
{
    float r2 = roughness * roughness;
    float a  = 1.0 - 0.5 * r2 / ( r2 + 0.33 );
    float b  = 0.45 * r2 / ( r2 + 0.09 );

    vec3  n2   = normalize ( n );
    vec3  l2   = normalize ( l );
    vec3  v2   = normalize ( v );

    float nl    = dot ( n2, l2 );
    float nv    = dot ( n2, v2 );
    vec3  lProj = normalize ( l2 - n2 * nl );
    vec3  vProj = normalize ( v2 - n2 * nv );
    float cx    = max ( dot ( lProj, vProj ), 0.0 );

    float cosAlpha = nl > nv ? nl : nv;
    float cosBeta  = nl > nv ? nv : nl;
    float dx       = sqrt ( ( 1.0 - cosAlpha * cosAlpha ) * ( 1.0 - cosBeta * cosBeta ) ) / cosBeta;

    gl_FragColor = max ( 0.0, nl ) * diffColor * (a + b * cx * dx);
}
