uniform mat4 mVP;
uniform mat4 mInvVP;
uniform mat4 mView;
uniform mat4 mProj;

in vec3 position;
in vec2 texcoord;
in vec3 iNormal;

out vec4 fColor;

struct Light {
  vec3 dir;
  vec3 color;
};

struct Material {
  vec3 albedo;
  float roughness;
  float metallic;
  float specular;
};

vec3 pbr(Material material, vec3 nrm, vec3 idir, vec3 odir) {
  float roughness = material.roughness;
  float metallic = material.metallic;
  float specular = material.specular;
  vec3 albedo = material.albedo;

  vec3 hdir = normalize(idir + odir);
  float NoH = max(0, dot(hdir, nrm));
  float NoL = max(0, dot(idir, nrm));
  float NoV = max(0, dot(odir, nrm));
  float VoH = clamp(dot(odir, hdir), 0, 1);
  float LoH = clamp(dot(idir, hdir), 0, 1);

  vec3 f0 = metallic * albedo + (1 - metallic) * 0.16 * specular * specular;
  vec3 fdf = f0 + (1 - f0) * pow(1 - VoH, 5);

  float k = (roughness + 1) * (roughness + 1) / 8;
  float vdf = 0.25 / ((NoV * k + 1 - k) * (NoL * k + 1 - k));

  float alpha2 = max(0, roughness * roughness);
  float denom = 1 - NoH * NoH * (1 - alpha2);
  float ndf = alpha2 / (denom * denom);

  vec3 brdf = fdf * vdf * ndf * f0 + (1 - f0) * albedo;
  return brdf * NoL;
}

vec3 calcRayDir(vec3 pos)
{
  vec4 vpos = mVP * vec4(pos, 1);
  vec2 uv = vpos.xy / vpos.w;
  vec4 ro = mInvVP * vec4(uv, -1, 1);
  vec4 re = mInvVP * vec4(uv, +1, 1);
  vec3 rd = normalize(re.xyz / re.w - ro.xyz / ro.w);
  return rd;
}

void main()
{
  vec3 normal = normalize(iNormal);
  vec3 viewdir = -calcRayDir(position);

  Material material;
  material.albedo = D_ALBEDO;
  material.roughness = D_ROUGHNESS;
  material.metallic = D_METALLIC;
  material.specular = 0.5;

  Light light;
  light.dir = normalize((mVP * vec4(-1, -2, 5, 0)).xyz);
  light.dir = faceforward(light.dir, -light.dir, normal);
  light.color = vec3(1, 1, 1);

  vec3 strength = pbr(material, normal, light.dir, viewdir);
  vec3 color = light.color * strength;
  fColor = vec4(color, 1.0);
}
