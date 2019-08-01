$input v_color0, v_texcoord0

#include "bgfx_shader.sh"

SAMPLER2D(s_texColor, 0);

#define SUPERSAMPLE

#if defined(OUTLINE)
  uniform float OutlineEdgeWidth;
  uniform vec4 OutlineColor;
#endif

#if defined(SHADOW)
  const float shadowBlur = 0.3;
  uniform vec4 ShadowOffset;
  uniform vec4 ShadowColor;
#endif

float contour(float dist, float edge, float width) {
  return clamp(smoothstep(edge - width, edge + width, dist), 0.0, 1.0);
}

float getSample(vec2 texCoords, float edge, float width) {
  return contour(texture2D(s_texColor, texCoords).r, edge, width);
}

void main() {  
  vec4 tex = texture2D(s_texColor, v_texcoord0);
  float dist  = tex.r;
  float width = fwidth(dist);
  vec4 textColor = clamp(v_color0, 0.0, 1.0);
  float outerEdge = 0.5;
  
  float alpha = contour(dist, outerEdge, width);
  #if defined(SUPERSAMPLE)
    float dscale = 0.354; // half of 1/sqrt2; you can play with this
    vec2 uv = v_texcoord0.xy;
    vec2 duv = dscale * (dFdx(uv) + dFdy(uv));
    vec4 box = vec4(uv - duv, uv + duv);

    float asum = getSample(box.xy, outerEdge, width)
               + getSample(box.zw, outerEdge, width)
               + getSample(box.xw, outerEdge, width)
               + getSample(box.zy, outerEdge, width);

    // weighted average, with 4 extra points having 0.5 weight each,
    // so 1 + 0.5*4 = 3 is the divisor
    alpha = (alpha + 0.5 * asum) / 3.0;
  #endif

  gl_FragColor = vec4(textColor.rgb, textColor.a * alpha);
  
  #if defined(OUTLINE)
    outerEdge = outerEdge - OutlineEdgeWidth * 0.2;

    float outlineOuterAlpha = clamp(smoothstep(outerEdge - width, outerEdge + width, dist), 0.0, 1.0);
    
    gl_FragColor.rgb = mix(OutlineColor.rgb, gl_FragColor.rgb, alpha);
    gl_FragColor.a = max(gl_FragColor.a, OutlineColor.a * outlineOuterAlpha);
  #endif
  
  #if defined(SHADOW)
    float shadowAlpha = clamp(smoothstep(max(outerEdge - shadowBlur, 0.05), outerEdge + shadowBlur, dist), 0.0, 1.0);
    vec4 shadow = ShadowColor * shadowAlpha;
    gl_FragColor = shadow * (1.0 - gl_FragColor.a) + gl_FragColor * gl_FragColor.a;
  #endif
}
