//! Normalized pixel coordinates.
varying vec2 vPixel;

void main (void)
{
  vPixel = vec2 ((occVertex.x + 1.0) * 0.5,
                 (occVertex.y + 1.0) * 0.5);

  vec4 aPosition = occVertex;

  gl_Position = occVertex;
}