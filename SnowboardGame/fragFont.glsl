varying vec2 vTextPos;
uniform sampler2D uTexUnit;
uniform vec4 uTextColor;
 
void main(void) {
  gl_FragColor = vec4(1, 1, 1, texture2D(uTexUnit, vTextPos).a) * uTextColor;
}