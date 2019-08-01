$input v_color0, v_texcoord0

#include "shaderlib.sh"

SAMPLER2D(s_texColor, 0);

void main()
{
    gl_FragColor = texture2D(s_texColor, v_texcoord0);

	if (gl_FragColor.a * v_color0.a < 0.1) discard;

	gl_FragColor = gl_FragColor * v_color0;
    gl_FragColor = mix(vec4(1,1,1,1), gl_FragColor, gl_FragColor.a);
}
