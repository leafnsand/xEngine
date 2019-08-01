$input a_position
$output v_texdir0

#include "shaderlib.sh"

void main()
{
	v_texdir0 = a_position;
	vec4 pos = mul(u_modelViewProj, vec4(a_position, 1.0) );
	gl_Position = pos.xyww;
}