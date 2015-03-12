attribute vec4 aTexCoord;
varying vec2 vTextPos;
 
void main(void) {
  gl_Position = vec4(aTexCoord.xy, 0, 1);
  vTextPos = aTexCoord.zw;
}