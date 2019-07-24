//////////////////////////////////////////////////////

@begin variable

<-color-> light_color;

<-texture2d-> texture;

<-float4x4-> model;
<-float4x4-> view;
<-float4x4-> projection;

<-float-> ambient;
<-float-> specular;
<-float-> diffuse; 

@end variable

//////////////////////////////////////////////////////

@begin vertex

<-in-> <-float3-> aPosition;
<-in-> <-float2-> aTexcoord0;
<-in-> <-float3-> aNormal;

<-out-> <-float2-> Texcoord;
<-out-> <-float3-> Normal;
<-out-> <-float3-> ViewPosition;

<-uniform-> <-float4x4-> uModel;
<-uniform-> <-float4x4-> uView;
<-uniform-> <-float4x4-> uProjection;

<-vs-> {
  Texcoord = aTexcoord0;
  <-float4x4-> MV_matrix = {- mul || uView || uModel -};
  Normal = {- mul || <-float3x3->(transpose(inverse(MV_matrix))) || aNormal -};
  <-float4-> ViewVector = {- mul || MV_matrix || <-float4->(aPosition, 1.0) -};
  ViewPosition = ViewVector.xyz;

  <-vs_position-> = {- mul || uProjection || ViewVector -};
}

@end vertex

//////////////////////////////////////////////////////

@begin fragment

<-in-> <-float2-> Texcoord;
<-in-> <-float3-> Normal;
<-in-> <-float3-> ViewPosition;

<-out-> <-float4-> <-fs_color->;

<-uniform-> <-sampler2d-> uTexture;

<-fs-> {
  <-float3-> light_color = <-float3->(1.0, 1.0, 1.0);

  <-float3-> light_direction = normalize(<-float3->(-1.0, 0.0, -1.0));

  <-float3-> normal = normalize(Normal);

  <-float3-> ambient = 0.1 * light_color;

  <-float3-> diffuse = 0.5 * max(dot(normal, -light_direction), 0.0) * light_color;

  <-float3-> specular = 0.6 * pow(max(dot(normal, normalize((-light_direction) + (-ViewPosition))), 0.0), 16) * light_color;

  <-float3-> result = ambient + diffuse + specular;

  <-fs_color-> = <-float4->(result, 1.0) * {- sample || uTexture || Texcoord -};
}

@end fragment

//////////////////////////////////////////////////////
