/*
 * GLUT Shapes Demo
 *
 * Written by Nigel Stewart November 2003
 *
 * This program is test harness for the sphere, cone
 * and torus shapes in GLUT.
 *
 * Spinning wireframe and smooth shaded shapes are
 * displayed until the ESC or q key is pressed.  The
 * number of geometry stacks and slices can be adjusted
 * using the + and - keys.
 */

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <windows.h>
#include <GL/glut.h>
#endif

#include <stdlib.h>

static int slices = 16;
static int stacks = 16;

/* GLUT callback Handlers */

void myDisplay (void)
{
glClear (GL_COLOR_BUFFER_BIT);//�����Ļ�ϵ���ɫ
glColor3f (1.0, 1.0, 0.0);//���õ�ǰ������ɫ
glBegin (GL_LINES);
glVertex2f (-0.5, 0.0);
glVertex2f (0.5, 0.0);
glEnd ();
glFlush();//Ҫ����,��Ȼ�������,������,��֤ǰ���OpenGL��������ִ��,�������ڻ������е���
}

int main(int argc, char *argv[])
{
glutInit(&argc, argv);//��ʼ��,�����ڵ�������GLUT����ǰ����һ��
glutInitDisplayMode (GLUT_RGBA | GLUT_SINGLE);//�趨ģʽ,RGBAɫ��,�͵�������
glutInitWindowPosition (100, 100);//���ô���λ��,�����-1,-1����Ĭ��λ��
glutInitWindowSize (500, 500);//���ô��ڴ�С
glutCreateWindow ("hello word!");//��������Ϊ"hello word!"�Ĵ���,���ڴ����󲻻�������ʾ����Ļ��,Ҫ���ú����glutMainLoop()�Ż���ʾ
glutDisplayFunc (myDisplay);//���û��ƺ���ʹ����ʾ�ڸմ����Ĵ�����
glutMainLoop();//��Ϣѭ��,���ڹرղŻ᷵��
return 0;
}
