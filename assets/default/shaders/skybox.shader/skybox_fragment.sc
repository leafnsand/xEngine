$input v_texdir0

#include "shaderlib.sh"

SAMPLERCUBE(s_texCube, 0);

void main()
{
	gl_FragColor = textureCube(s_texCube, v_texdir0);
}
