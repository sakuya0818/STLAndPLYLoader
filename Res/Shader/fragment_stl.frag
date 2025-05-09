#version 330 core
out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
}; 

struct Light {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;  
in vec3 Normal;  
  
uniform vec3 viewPos;
uniform Material material;
uniform Light light1;
uniform Light light2;

uniform vec3 highLightColor;
uniform float isHighLight;

vec3 CalcDirLight(Light light, vec3 normal, vec3 viewDir);

void main()
{
    // 计算正常光照颜色
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = CalcDirLight(light1, norm, viewDir);
    result += CalcDirLight(light2, norm, viewDir);

    // 使用mix函数线性混合颜色（代替if判断）
    vec3 finalColor = mix(result, highLightColor, isHighLight);

    FragColor = vec4(finalColor, 1.0);
}

vec3 CalcDirLight(Light light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // 漫反射着色
    float diff = max(dot(normal, lightDir), 0.0);
    // 镜面光着色
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // 合并结果
    vec3 ambient  = light.ambient * material.ambient;
    vec3 diffuse  = light.diffuse  * (diff * material.diffuse);
    vec3 specular = light.specular * (spec * material.specular); 
    return (ambient + diffuse + specular);
}
