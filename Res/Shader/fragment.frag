#version 330 core
in vec3 ourColor; // 从顶点着色器接收颜色

out vec4 FragColor;
 
void main()
{
     //0.2f, 0.8f, 0.8f 高级青
     FragColor =  vec4(ourColor, 1.0);;
}
