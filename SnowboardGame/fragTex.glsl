//
// Author: Evan Peterson
// Cal Poly, CPE-471, Winter 2014
//

uniform sampler2D uTexUnit;

struct Material {
   vec3 aColor;
   vec3 dColor;
   vec3 sColor;
   float shine;
};

uniform vec3 uLightColor;
uniform vec3 uLightDir;
uniform Material uMat;

varying vec2 vTexCoord;
varying vec3 vNormal;
varying vec3 vEye;
varying vec3 vPosition;

void main() {
   float dist, diffuse, spec;
   vec3 normal, eye, light, refl, color;
   //vec4 texColor = texture2D(uTexUnit, vTexCoord);

   normal = normalize(vNormal);
   eye = normalize(vEye);

   light = normalize(uLightDir);

   refl = normalize(-light + 2.0 * dot(light, normal) * normal);

   diffuse = max(dot(normal, light), 0.0);
   spec = pow(max(dot(eye, refl), 0.0), uMat.shine);

   // Compute the specular and diffuse components for the light.
   color = uLightColor * (diffuse * uMat.dColor + spec * uMat.sColor);

   // Add the ambient component.
   color += uLightColor * uMat.aColor;

   //gl_FragColor = vec4(texColor[0] + color.r,
   //                    texColor[1] + color.b,
   //                    texColor[2] + color.g, 1);
   gl_FragColor = vec4(color.r, color.b, color.g, 1);
}
