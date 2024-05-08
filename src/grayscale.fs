#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables
uniform int test[8];
uniform vec2 res;
uniform float runTime;
uniform vec3 viewPos;
uniform vec3 viewCenter;


// vec3 render( in vec3 ro, in vec3 rd )
// {
//     vec3 col = vec3(0.7, 0.9, 1.0) +rd.y*0.8;
//     vec2 res = castRay(ro,rd);
//     float t = res.x;
//     float m = res.y;
//     if( m>-0.5 )
//     {
//         vec3 pos = ro + t*rd;
//         vec3 nor = calcNormal( pos );
//         vec3 ref = reflect( rd, nor );

//         // material
//         col = 0.45 + 0.35*sin( vec3(0.05,0.08,0.10)*(m-1.0) );
//         if( m<1.5 )
//         {

//             float f = checkersGradBox( 5.0*pos.xz );
//             col = 0.3 + f*vec3(0.1);
//         }

//         // lighting
//         float occ = calcAO( pos, nor );
//         vec3  lig = normalize( vec3(cos(-0.4 * runTime), sin(0.7 * runTime), -0.6) );
//         vec3  hal = normalize( lig-rd );
//         float amb = clamp( 0.5+0.5*nor.y, 0.0, 1.0 );
//         float dif = clamp( dot( nor, lig ), 0.0, 1.0 );
//         float bac = clamp( dot( nor, normalize(vec3(-lig.x,0.0,-lig.z))), 0.0, 1.0 )*clamp( 1.0-pos.y,0.0,1.0);
//         float dom = smoothstep( -0.1, 0.1, ref.y );
//         float fre = pow( clamp(1.0+dot(nor,rd),0.0,1.0), 2.0 );

//         dif *= calcSoftshadow( pos, lig, 0.02, 2.5 );
//         dom *= calcSoftshadow( pos, ref, 0.02, 2.5 );

//         float spe = pow( clamp( dot( nor, hal ), 0.0, 1.0 ),16.0)*
//                     dif *
//                     (0.04 + 0.96*pow( clamp(1.0+dot(hal,rd),0.0,1.0), 5.0 ));

//         vec3 lin = vec3(0.0);
//         lin += 1.30*dif*vec3(1.00,0.80,0.55);
//         lin += 0.40*amb*vec3(0.40,0.60,1.00)*occ;
//         lin += 0.50*dom*vec3(0.40,0.60,1.00)*occ;
//         lin += 0.50*bac*vec3(0.25,0.25,0.25)*occ;
//         lin += 0.25*fre*vec3(1.00,1.00,1.00)*occ;
//         col = col*lin;
//         col += 10.00*spe*vec3(1.00,0.90,0.70);

//         col = mix( col, vec3(0.8,0.9,1.0), 1.0-exp( -0.0002*t*t*t ) );
//     }

//     return vec3( clamp(col,0.0,1.0) );
// }

vec3 castRay(in vec3 ro, in vec3 rd) {
    // ew
    ivec2 foo = ivec2(mod(fragTexCoord.x*16, 4), mod(fragTexCoord.y*8, 2));
    int steps = test[foo.x + foo.y * 4];
    vec3 pos = ro;

    // vec3 col = steps*vec3(0.7/8., 0.9/8., 1.0/8.);

    // vec3 localPos = mod(pos, 1.);
    // float closestEdge = min(min(localPos.x, localPos.y), localPos.z);
    for(int i=0; i<30; i++) {
    pos += rd;
    }


    vec3 col = mod(pos, 30.)/30.;
    if (ivec2(pos.xy) == ivec2(mod(pos.x*4, 4), mod(fragTexCoord.y*2,  
    
     2))) {
        col = vec3(0);
    }
    if (pos.x < -0) {
        col.x = 0;
    }
    if (pos.y < -0) {
        col.y = 0;
    }
    if (pos.z < -0) {
        col.z = 0;
    }

    return col; // mod(rd, 1.); // vec3(0.7, 0.9, 1.0);
}

mat3 setCamera( in vec3 ro, in vec3 ta, float cr )
{
    vec3 cw = normalize(ta-ro);
    vec3 cp = vec3(sin(cr), cos(cr),0.0);
    vec3 cu = normalize( cross(cw,cp) );
    vec3 cv = normalize( cross(cu,cw) );
    return mat3( cu, cv, cw );
}

void main()
{
    vec2 p = (-res.xy + 2.0*gl_FragCoord.xy)/res.y;

    vec3 ro = viewPos;
    vec3 ta = viewCenter;

    // ca = camera-to-world transformation
    // setCamera( ro, ta, 0.0 ) returns a 3x3 matrix that transforms
    // a coordinate from the camera's coordinate system to the world's
    // coordinate system. The first two operands, ro and ta, are the
    // position and target of the camera, respectively. The last operand,
    // 0.0, is the camera's rotation angle around the y-axis (in radians).
    mat3 ca = setCamera( ro, ta, 0.0 );
    // ray direction
    vec3 rd = ca * normalize( vec3(p.xy,2.0) );

    // // render
    vec3 col = castRay( ro, rd );// render( ro, rd );

    // gamma
    // col = pow( col, vec3(0.4545) );

    vec3 tot = col;// vec3(0.299, 0.587, 0.114);

    finalColor = vec4(tot, 1.0);
    // vec2 tc = fragTexCoord*res;

    // float dist = length(tc);

    // // Texel color fetching from texture sampler
    // vec4 texelColor = texture(texture0, fragTexCoord)*colDiffuse*fragColor;

    // // Convert texel color to grayscale using NTSC conversion weights
    // // float gray = dot(texelColor.rgb, vec3(0.299, 0.587, 0.114));

    // ivec2 foo = ivec2(fragTexCoord.x*4, fragTexCoord.y*2);

    // vec2 col = vec2(float(test[foo.x + foo.y*4])/8.);

    // // Calculate final fragment color
    // finalColor = vec4(col, 0, texelColor.a);
}