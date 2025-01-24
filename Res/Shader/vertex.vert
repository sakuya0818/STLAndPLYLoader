#version 330 core
layout (location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor; // 颜色

out vec3 ourColor; // 向片段着色器传递颜色
 
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection*view*model*vec4(aPos,1.0f);
    ourColor = aColor;
}
