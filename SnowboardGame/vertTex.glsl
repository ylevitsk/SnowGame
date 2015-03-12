//
// Author: Evan Peterson
// Cal Poly, CPE-471, Winter 2014
//

uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform vec3 uCamPos;

attribute vec2 aTexCoord;
attribute vec3 aPosition;
attribute vec3 aNormal;

varying vec2 vTexCoord;
varying vec3 vNormal;
varying vec3 vEye;
varying vec3 vPosition;

varying vec3 vColor;

void main() {
   vec4 position;

   /* Model Transform */
   position = uModelMatrix * vec4(aPosition, 1.0);

   /* Lighting */
   vNormal = vec3(uModelMatrix * vec4(aNormal, 0.0)); // Transform the normal
   vPosition = vec3(position);

   // Calculate Refl using L-vec and N-vec in world-space
   // Then calculate eye-vec/view-vec in camera-space??
   // view_vector = normalize((0, 0, 0) - vPos) // camera at origin?
   // R(view) = uViewMatrix * vec4(refl, 0.0); ???

   //vEye = -uCamPos - vPosition;
   position = uViewMatrix * position;
   vEye = -uCamPos - vec3(position);

   /* Texturing */
//   vTexCoord = aTexCoord;

   /* Camera and Projection Transforms */
   //gl_Position = uProjMatrix * uViewMatrix * position;
   gl_Position = uProjMatrix * position;

   //vColor = uCamPos;
}
