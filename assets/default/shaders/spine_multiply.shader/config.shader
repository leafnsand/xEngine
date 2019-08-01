<Shader name="default" queue="Geometry" var="varing.def.sc"> 
	<Property name="s_texColor" type="2D" default="white"/> 
	<VertexShader name="vs" src="vs_spine.sc"/> 
	<FragmentShader name="fs" src="fs_spine_multiply.sc"/> 
	<RenderState name="rs" Cull="Off" ZWrite="On" ZTest="Less" Blend="SrcAlpha OneMinusSrcAlpha" BlendOp="Add" ColorMask="RGB"/> 
	<Pass name="pass" feature="" vs="vs" fs="fs" rs="rs"/>
</Shader>