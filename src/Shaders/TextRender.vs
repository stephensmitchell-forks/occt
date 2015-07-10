varying vec2 TexCoord;

void main (void)
{
  TexCoord = occTexCoord.st;

  // Do fixed functionality vertex transform
  gl_Position = occProjectionMatrix * occWorldViewMatrix * occModelWorldMatrix * occVertex;
}