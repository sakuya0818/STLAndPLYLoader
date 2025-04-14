#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;  
out vec3 Normal;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    // 计算经过模型变换后的世界坐标，传给片段着色器
    FragPos = vec3(model * vec4(aPos, 1.0));
//    Normal = vec3(projection*view*model * vec4(aNormal, 1.0));//这样 暗部 才会有透视的情况，才会看起来正常一点
    // 随着面的转动，修改法向量一起变化
    Normal = mat3(transpose(inverse(model))) * aNormal;
}
 
