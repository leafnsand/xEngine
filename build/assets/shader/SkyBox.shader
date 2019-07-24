//////////////////////////////////////////////////////

@begin vertex

<-in-> <-float3-> aPosition;

<-out-> <-float3-> Texcoord;

<-uniform-> <-float4x4-> uView;
<-uniform-> <-float4x4-> uProjection;

<-vs-> {
  Texcoord = aPosition;

  <-float4-> View = {- mul || uView || <-float4->(aPosition, 1.0) -};

  <-float4-> Position = {- mul || uProjection || View -};

  <-vs_position-> = Position.xyww;
}

@end vertex

//////////////////////////////////////////////////////

@begin fragment

<-in-> <-float3-> Texcoord;

<-out-> <-float4-> <-fs_color->;

<-uniform-> <-samplerCube-> uTexture;

<-fs-> {
  <-fs_color-> = {- sample || uTexture || Texcoord -};
}

@end fragment

//////////////////////////////////////////////////////
