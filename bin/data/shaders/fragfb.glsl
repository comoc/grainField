uniform sampler2D texture0;
uniform sampler2D texture1;

uniform float width;
uniform float height;
uniform float focalOffset;

float step_w = 1.0 / width;
float step_h = 1.0 / height;

const float focalRange = 0.5;

void main()
{
	vec4 depthTex = texture2D(texture1, gl_TexCoord[0].xy);
	float blur;
	if (focalOffset < depthTex.r)
		blur = clamp((depthTex.r - focalOffset) / focalRange, 0.0, 1.0);
	else
		blur = clamp((focalOffset - depthTex.r) / focalRange, 0.0, 1.0);
	
	blur = 5.0 * blur;

	float blur_w = blur * step_w;
	float blur_h = blur * step_h;
    vec4 texCol0LT = texture2D(texture0, gl_TexCoord[0].xy + vec2(-blur_w, blur_h));
    vec4 texCol0T = texture2D(texture0, gl_TexCoord[0].xy + vec2(0.0, blur_h));
    vec4 texCol0RT = texture2D(texture0, gl_TexCoord[0].xy + vec2(blur_w, blur_h));
    vec4 texCol0L = texture2D(texture0, gl_TexCoord[0].xy + vec2(-blur_w, 0.0));
    vec4 texCol0 = texture2D(texture0, gl_TexCoord[0].xy);
    vec4 texCol0R = texture2D(texture0, gl_TexCoord[0].xy + vec2(blur_w, 0.0));
    vec4 texCol0LB = texture2D(texture0, gl_TexCoord[0].xy + vec2(-blur_w, -blur_h));
    vec4 texCol0B = texture2D(texture0, gl_TexCoord[0].xy + vec2(0.0, -blur_h));
    vec4 texCol0RB = texture2D(texture0, gl_TexCoord[0].xy + vec2(blur_w, -blur_h));
	
	vec4 col = (texCol0LT + texCol0RT + texCol0LB + texCol0RB) / 16.0
		+ (texCol0T + texCol0L + texCol0R + texCol0B) / 8.0
		+ texCol0 / 4.0;
	
	float y = (0.30 * col.r + 0.59 * col.g + 0.11 * col.b);
	

	col  = clamp(col * (y + 1.0) * (y + 1.0), 0.0, 1.0);

	gl_FragColor = col;
#if 0 
	gl_FragColor.r = clamp(blur, 0.0, 1.0);
	gl_FragColor.g = 0.0;
	gl_FragColor.b = 0.0;
	gl_FragColor.a = 1.0;
#endif
} 


