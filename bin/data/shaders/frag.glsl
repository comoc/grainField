uniform sampler2D texture0;
uniform sampler2D texture1;
uniform float colorGain;
uniform float textureBlend;
uniform float peakLeftRight;

void main()
{
	vec4 peakOffset = vec4(peakLeftRight, peakLeftRight, peakLeftRight, 0.0);
	
	vec4 col0 = texture2D(texture0, gl_TexCoord[0].xy);
	vec4 col1 = texture2D(texture1, gl_TexCoord[0].xy);

	vec4 col = clamp(peakOffset + colorGain * gl_Color * ((1.0 - textureBlend) * col0 + textureBlend * col1), 0.0, 1.0);

 	gl_FragColor = col;
	//float y = col.r * 0.257 + col.g * 0.504 + col.b * 0.098 + 16.0;
 	//gl_FragColor = vec4(y * 0.01, y, y * 0.5, col.a);
} 

