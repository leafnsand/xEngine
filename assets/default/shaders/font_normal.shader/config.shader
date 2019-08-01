<Shader name="font_normal" queue="Geometry" var="varing.def.sc"> 
	<Property name="s_texColor" type="2d" default="white"/> 
	<VertexShader name="vs" src="vs_fairy.sc"/> 
	<FragmentShader name="fs" src="fs_font_normal.sc"/> 
	<RenderState name="rs" Cull="Off" ZWrite="Off" Blend="SrcAlpha OneMinusSrcAlpha" BlendOp="Add" ColorMask="RGB"/> 
	<Pass name="pass" feature="" vs="vs" fs="fs" rs="rs"/>
</Shader>