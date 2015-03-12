//
// Author: Evan Peterson
// Cal Poly, CPE-471, Winter 2014
//

struct Material {
   vec3 aColor;
   vec3 dColor;
   vec3 sColor;
   float shine;
};

uniform vec3 uLightColor;
uniform vec3 uLightDir;
uniform Material uMat;

varying vec3 vNormal;
varying vec3 vEye;
varying vec3 vPosition;

void main() {
   float dist, diffuse, spec;
   vec3 normal, eye, light, refl, color;

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

   gl_FragColor = vec4(color.r,
                       color.g,
                       color.b, 1.0);
}
