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

	//float y = col.x * 0.257 + col.y * 0.504 + col.z * 0.098 + 16.0;
 	gl_FragColor = col;//vec4(0, y, 0, col.w);
} 

