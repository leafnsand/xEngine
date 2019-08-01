<Shader name="fairy_mask" queue="Geometry" var="varing.def.sc"> 
	<VertexShader name="vs" src="vs_fairy.sc"/> 
	<FragmentShader name="fs" src="fs_fairy_mask.sc"/> 
	<RenderState name="rs" Cull="Off" ZWrite="Off" Blend="SrcAlpha OneMinusSrcAlpha" BlendOp="Add" ColorMask="RGBA"/> 
	<Pass name="pass" feature="" vs="vs" fs="fs" rs="rs"/>
</Shader>