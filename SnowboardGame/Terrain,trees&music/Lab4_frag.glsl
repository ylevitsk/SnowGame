struct Material {
  vec3 aColor;
  vec3 dColor;
  vec3 sColor;
  float shine;
};

uniform Material uMat;
uniform vec3 uLightColor;

varying vec3 vNormal;
varying vec4 vLight;
varying vec4 vView;
varying vec4 vRefl;

void main() {
   vec3 Diffuse, Spec;
   vec4 Refl;
   vec4 norm = vec4(vNormal, 0);

   Refl = -vLight + 2.0 * (dot(vLight, norm)) * norm;
   Refl = normalize(Refl);
   

   Diffuse = uLightColor * clamp(dot(vec4(normalize(vNormal), 1), normalize(vLight)), 0.0, 1.0) * uMat.dColor;
   Spec = uLightColor * pow(clamp(dot(normalize(vView), normalize(Refl)), 0.0, 1.0), uMat.shine) * uMat.sColor;
   gl_FragColor = vec4(Diffuse + Spec + uLightColor * uMat.aColor, 1);
}


