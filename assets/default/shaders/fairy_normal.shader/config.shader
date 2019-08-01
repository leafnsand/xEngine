<Shader name="fairy_normal" queue="Transparent" var="varing.def.sc"> 
	<Property name="s_texColor" type="2d" default="white"/> 
	<VertexShader name="vs" src="vs_fairy.sc"/> 
	<FragmentShader name="fs" src="fs_fairy_normal.sc"/> 
	<RenderState name="rs" Cull="Off" ZWrite="Off" Blend="SrcAlpha OneMinusSrcAlpha" BlendOp="Add" ColorMask="RGB"/> 
	<Pass name="pass" feature="" vs="vs" fs="fs" rs="rs"/>
</Shader>