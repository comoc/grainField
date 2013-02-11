uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;

//uniform vec4 viewport;
//uniform vec3 forcePoint;
uniform float displacementGain;
uniform float soundGain;
uniform float soundPeakGain;

uniform float low;
uniform float mid;
uniform float high;
uniform float pointSize;
uniform float textureBlend;
uniform float noiseGain;
uniform float zClip;
uniform float zOffset;
uniform float timeFraction;
uniform float aspectRatio;

//varying float depth;

const float PI = 3.14159265359;
const float TWO_PI = 6.28318530718;
const float HALF_PI = 1.570796326794897;

/*
vec3 win2normdev(vec3 win)
{
    win.x = (win.x - viewport.x) / viewport.z;
    win.y = (win.y - viewport.y) / viewport.w;

    win.x = win.x * 2.0 - 1.0;
    win.y = win.y * 2.0 - 1.0;
    win.z = win.z * 2.0 - 1.0;

	return win;
}

vec3 unproject(vec3 win)
{
	vec4 obj;

	win = win2normdev(win);

	obj = gl_ModelViewProjectionMatrixInverse * vec4(win, 1.0);
    obj.x = obj.x / obj.w;
    obj.y = obj.y / obj.w;
    obj.z = obj.z / obj.w;

	return obj.xyz;
}
*/


void main(void)
{
	vec4 disp;
	vec4 t0;
	vec4 t1;
	float df0;
	float df1;
	float dist;
    float pointSizeGain;
	vec4 pos;
	vec3 ndp;
	vec3 posndp;
	vec4 posMV;
//	float len;
    float alpha;
    float sinXZ;
    float bx;
    float by;
	float ft;
	//vec3 fp = unproject(forcePoint);

	vec4 noise2d;
	vec4 noise;

	vec2 peak2d;
	float peaktcs;

    alpha = 1.0;

    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;


	t0 = texture2D(texture0, gl_TexCoord[0].st);
	t1 = texture2D(texture1, gl_TexCoord[0].st);
	noise2d = texture2D(texture2, gl_TexCoord[0].st) - vec4(0.5, 0.5, 0.5, 0.5);
	noise2d = noiseGain * noise2d;
	noise = vec4(noise2d.r, noise2d.a, 0.0, 0.0);

	peaktcs = length((gl_TexCoord[0].st - vec2(0.5, 0.5)) * vec2(aspectRatio, 1.0));
	//peak2d = vec2(0, texture2D(texture3, vec2(peaktcs, 0.0)).r);
	peak2d = vec2(0, texture2D(texture3, vec2(1.0 - peaktcs, 0.0)).r);

	//noise = texture2D(texture2, gl_TexCoord[0].st);
	//noise = 0.25 * (2.0 * noise - vec4(1.0, 1.0, 1.0, 1.0));
	//noise.w = 0.0;

	df0 = (0.30 * t0.r + 0.59 * t0.g + 0.11 * t0.b);
	df1 = (0.30 * t1.r + 0.59 * t1.g + 0.11 * t1.b);
	
	//dist = (1.0 - textureBlend) * df0 + textureBlend * df1;
	dist = mix(df0, df1, textureBlend);
	if (dist < 0.25) {
		dist = 0.0;
	} else {
		if (dist < 0.5) {
			dist = 0.2;
		} else {
			if (dist < 0.75) {
				dist = 0.6;
			} else {
				dist = 1.0;
			}
		}
	}
    pointSizeGain = pow(4.0, dist - 0.75);

	if (dist < 0.3)
		dist += soundGain * low;
	else if (dist > 0.9)
		dist += soundGain * high;
	else
		dist += soundGain * mid;

	disp = gl_Vertex + vec4(gl_Normal * dist * displacementGain, 0.0);
	disp += noise;

	ft = timeFraction * 2.0 - 1.0;
	ft *= 1024.0;
	
	bx = (mod(ft, 13.0) / 12.0) * 0.001;
	by = (mod(ft, 17.0) / 16.0) * 0.001;
	disp.x += bx * soundGain;
	disp.y += by * soundGain;

	disp.xy += peak2d * soundPeakGain;

    if (disp.z < zClip)
        alpha = 0.0;
	

	disp.z = disp.z - zOffset;
    pos = gl_ModelViewProjectionMatrix * disp;
	
//	depth = pos.z / pos.w;
    

#if 0 
	posndp = pos.xyz / pos.w;
	ndp = win2normdev(forcePoint);
	dist = distance(ndp.xy, posndp.xy);
	if (dist > 0.5)
		dist = 0.5;
	dist = dist / 0.5;
	dist = sin(HALF_PI * dist);
	dist *= dist;

	posndp.xy = posndp.xy * dist + ndp.xy * (1.0 - dist);
	pos.xyz = posndp * pos.w;
#endif

	gl_Position = pos;

	gl_FrontColor.rgb = gl_Color.rgb;
	gl_FrontColor.a = gl_Color.a * alpha;

	posMV = gl_ModelViewMatrix * disp;
	

//	len = length(posMV);
//	len *= len;
	gl_PointSize = pointSize * pointSizeGain;
//	gl_PointSize = clamp(pointSize / len, 1.0, 1024.0);
}

