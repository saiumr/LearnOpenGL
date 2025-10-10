#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    // gl_Position��clip space�У�����͸�ӳ���ת����NDC��zֵ����Ϊ���
    // ͸�ӳ������ڶ�����ɫ������֮��ִ�еģ���gl_Position��xyz�������w����
    // ���ｫz��������Ϊw������ȷ�����ֵΪ1.0��Զ�ü��棩��������պ���������Զ��
    // ���������Ա�����պ��ڵ���������
    gl_Position = pos.xyww;
}
