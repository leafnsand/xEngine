<Shader name="skybox" queue="Geometry" var="varing.def.sc"> 
	<Property name="s_texColor" type="Cube" default="white"/> 
	<VertexShader name="vs" src="skybox_vertex.sc"/> 
	<FragmentShader name="fs" src="skybox_fragment.sc"/> 
	<RenderState name="rs" Cull="Off" ZWrite="On" ZTest="Lequal" Blend="SrcAlpha OneMinusSrcAlpha" BlendOp="Add" ColorMask="RGB"/> 
	<Pass name="pass" feature="" vs="vs" fs="fs" rs="rs"/>
</Shader>