<Shader name="default" queue="Geometry" var="varing.def.sc"> 
	<Property name="s_texColor" type="2D" default="white"/> 
	<VertexShader name="vs" src="vertex.sc"/> 
	<FragmentShader name="fs" src="fragment.sc"/> 
	<RenderState name="rs" Cull="Off" ZWrite="On" ZTest="Less" Blend="SrcAlpha OneMinusSrcAlpha" BlendOp="Add" ColorMask="RGB"/> 
	<Pass name="pass" feature="" vs="vs" fs="fs" rs="rs"/>
</Shader>