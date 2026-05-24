#version 330 core

// ─── Structs ─────────────────────────────────────────────────────────────────

struct Material {
    vec3  ambient;
    vec3  diffuse;
    vec3  specular;
    float shininess;
};

struct DirectionalLight {
    vec3 direction;
    vec3 color;
};

struct PointLight {
    vec3  position;
    vec3  color;
    float constant;
    float linear;
    float quadratic;
};

// ─── Uniforms ─────────────────────────────────────────────────────────────────

#define MAX_POINT_LIGHTS 8

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3         viewPos;
uniform Material     material;
uniform DirectionalLight dirLight;
uniform int          numPointLights;
uniform PointLight   pointLights[MAX_POINT_LIGHTS];

// ─── Light functions ─────────────────────────────────────────────────────────

vec3 calcDirectional(DirectionalLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);

    // Blinn-Phong specular
    vec3  halfVec = normalize(lightDir + viewDir);
    float spec    = pow(max(dot(normal, halfVec), 0.0), material.shininess);

    vec3 ambient  = light.color * material.ambient;
    vec3 diffuse  = light.color * diff  * material.diffuse;
    vec3 specular = light.color * spec  * material.specular;

    return ambient + diffuse + specular;
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3  lightDir = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);
    float atten    = 1.0 / (light.constant
                           + light.linear    * distance
                           + light.quadratic * distance * distance);

    float diff    = max(dot(normal, lightDir), 0.0);
    vec3  halfVec = normalize(lightDir + viewDir);
    float spec    = pow(max(dot(normal, halfVec), 0.0), material.shininess);

    vec3 ambient  = light.color * material.ambient  * atten;
    vec3 diffuse  = light.color * diff * material.diffuse  * atten;
    vec3 specular = light.color * spec * material.specular * atten;

    return ambient + diffuse + specular;
}

// ─── Main ─────────────────────────────────────────────────────────────────────

void main() {
    vec3 norm    = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = calcDirectional(dirLight, norm, viewDir);

    for (int i = 0; i < numPointLights; ++i)
        result += calcPointLight(pointLights[i], norm, FragPos, viewDir);

    // Gamma correction (2.2)
    result = pow(result, vec3(1.0 / 2.2));

    FragColor = vec4(result, 1.0);
}
