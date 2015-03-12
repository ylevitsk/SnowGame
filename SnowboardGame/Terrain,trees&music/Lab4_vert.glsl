struct Material {
  vec3 aColor;
  vec3 dColor;
  vec3 sColor;
  float shine;
};

attribute vec3 aPosition;
attribute vec3 aNormal;

uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform vec3 uCameraPos;
uniform vec3 uLightPos;
uniform vec3 uLightColor;
uniform Material uMat;

varying vec3 vNormal;
varying vec4 vLight;
varying vec4 vView;

void main() {
   vec4 vPosition;
   vec4 Light;
   vec4 Refl, View;
   vec3 Spec, Diffuse;
   vec4 transNormal;

   vPosition = uModelMatrix * vec4(aPosition.x, aPosition.y, aPosition.z, 1);

   transNormal = uModelMatrix * vec4(aNormal, 0);
   transNormal = normalize(transNormal);

   Light = vec4(uLightPos, 1) - vPosition;
   Light = normalize(Light);

   View = vec4(uCameraPos, 1) - vPosition;
   View = normalize(View);

   vNormal = vec3(transNormal.x, transNormal.y, transNormal.z);
   vLight = Light;
   vView = View;

   vPosition = uViewMatrix * vPosition;
   gl_Position = uProjMatrix * vPosition;
}