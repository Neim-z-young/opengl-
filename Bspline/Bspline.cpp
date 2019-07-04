#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include <gl\glui.h>
#include <math.h>
#include "bObj.h"
#include "common.h"

int g_xform_mode = TRANSFORM_NONE;
int   g_main_window;
double g_windows_width, g_windows_height;

Bspline b_obj;

//the lighting
static GLfloat g_light0_ambient[] =  {0.0f, 0.0f, 0.0f, 1.0f};//环境光
static GLfloat g_light0_diffuse[] =  {1.0f, 1.0f, 1.0f, 1.0f};//散射光
static GLfloat g_light0_specular[] = {1.0f,1.0f,1.0f,1.0f}; //镜面光
static GLfloat g_light0_position[] = {0.0f, 0.0f, 100.0f, 0.0f};//光源的位置。第4个参数为1，表示点光源；第4个参数量为0，表示平行光束{0.0f, 0.0f, 10.0f, 0.0f}

static GLfloat g_material[] = {0.96f, 0.8f, 0.69f, 1.0f};//材质
static GLfloat g_rquad = 0;

static float g_x_offset   = 0.0;
static float g_y_offset   = 0.0;
static float g_z_offset   = 0.0;
static float g_scale_size = 1; 
static int  g_press_x; //鼠标按下时的x坐标
static int  g_press_y; //鼠标按下时的y坐标

const int n = 1000;
const GLfloat R = 0.5f;
const GLfloat Pi = 3.1415926536f;
int g_view_type = VIEW_FLAT;
int g_draw_content = SHAPE_TRIANGLE;


#define NUM_POINTS 6
#define NUM_SEGMENTS (NUM_POINTS-3) 

struct Point2
{
	double x;
	double y;

	Point2() { ; }
	Point2(int px, int py) { x = px; y = py; }
	void SetPoint2(int px, int py) { x = px; y = py; }
};
/*全局变量*/
Point2 vec[NUM_POINTS];
/*绘制3次B样条曲线*/
void DrawBspline(int n)
{
	float f1, f2, f3, f4;
	float deltaT = 1.0 / n;
	float T;
	
	vec[0].SetPoint2(2, 4);
	vec[1].SetPoint2(1, 3);
	vec[2].SetPoint2(2, 2);
	vec[3].SetPoint2(2.5, 3);
	vec[4].SetPoint2(4, 2);
	vec[5].SetPoint2(4, 4);

	glLineWidth(1.5f);
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < NUM_POINTS; i++)
	{
		glVertex2f(vec[i].x, vec[i].y);
	}
	glEnd();

	glPointSize(5.0f);
	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_POINTS);
	for (int i = 0; i < NUM_POINTS; i++)
	{
		glVertex2f(vec[i].x, vec[i].y);
	}
	glEnd();

	glLineWidth(2.f);
	glBegin(GL_LINE_STRIP);
	for (int num = 0; num < NUM_SEGMENTS; num++)
	{
		for (int i = 0; i <= n; i++) {

			T = i * deltaT;

			f1 = (-T * T*T + 3 * T*T - 3 * T + 1) / 6.0;
			f2 = (3 * T*T*T - 6 * T*T + 4) / 6.0;
			f3 = (-3 * T*T*T + 3 * T*T + 3 * T + 1) / 6.0;
			f4 = (T*T*T) / 6.0;

			glVertex2f(f1*vec[num].x + f2 * vec[num + 1].x + f3 * vec[num + 2].x + f4 * vec[num + 3].x,
				f1*vec[num].y + f2 * vec[num + 1].y + f3 * vec[num + 2].y + f4 * vec[num + 3].y);
		}
	}

	glEnd();
}

//TODO 通过动态规划计算的N值 OK 
void DrawBspline(Bspline &b_obj)
{
	glLineWidth(2.f);
	glBegin(GL_LINE_STRIP);
	//glNormal3f(0.0f, 0.0f, 1.0f);  //指定面法向
	for (int i = 0; i < b_obj.cp.size(); i++)
	{
		//glVertex3f(b_obj.cp[i].fX, b_obj.cp[i].fY, 0.f);
		glVertex2f(b_obj.cp[i].fX, b_obj.cp[i].fY);
	}
	glEnd();

	glPointSize(10.0f);
	glBegin(GL_POINTS);
	//glNormal3f(0.0f, 0.0f, 1.0f);  //指定面法向
	for (int i = 0; i < b_obj.cp.size(); i++)
	{
		//glVertex3f(b_obj.cp[i].fX, b_obj.cp[i].fY, 0.f);
		glVertex2f(b_obj.cp[i].fX, b_obj.cp[i].fY);
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
//	glNormal3f(0.0f, 0.0f, 1.0f);  //指定面法向
	float delta = 1.f / b_obj.u.size();
	int t = b_obj.degree;         // 控制矢量区间的左边界，与样条次数相关
	for (int k = 0; k < b_obj.seg; k++)       //按段数绘制
	{
		for (int j = b_obj.k * t+1; j < b_obj.k * (t+1); j++) {  //每段对应的控制点范围
			Vector2 points(0, 0);
			for (int i = k; i < k+b_obj.n; i++)       //根据N函数值来计算个点
			{
				points += (b_obj.cp[i] * b_obj.N[i-k][b_obj.degree][j]);
			}
			glVertex2f(points.fX, points.fY);
		}
	}
	glEnd();
}

//TODO 通过递归计算的N值 OK
void DrawBspline1(Bspline &b_obj)
{
	glLineWidth(2.f);
	glBegin(GL_LINE_STRIP);
	//glNormal3f(0.0f, 0.0f, 1.0f);  //指定面法向
	for (int i = 0; i < b_obj.cp.size(); i++)
	{
		//glVertex3f(b_obj.cp[i].fX, b_obj.cp[i].fY, 0.f);
		glVertex2f(b_obj.cp[i].fX, b_obj.cp[i].fY);
	}
	glEnd();

	glPointSize(10.0f);
	glBegin(GL_POINTS);
	//glNormal3f(0.0f, 0.0f, 1.0f);  //指定面法向
	for (int i = 0; i < b_obj.cp.size(); i++)
	{
		//glVertex3f(b_obj.cp[i].fX, b_obj.cp[i].fY, 0.f);
		glVertex2f(b_obj.cp[i].fX, b_obj.cp[i].fY);
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	float delta = 1.f / b_obj.u.size();
	int t = b_obj.degree;         // 控制矢量区间的左边界，与样条次数相关
	for (int k = 0; k < b_obj.seg; k++)       //按段数绘制
	{
		for (int j = b_obj.k * t+1; j < b_obj.k * (t + 1); j++) {  //每段对应的控制点范围
			Vector2 points(0, 0);
			for (int i = k; i < k + b_obj.n; i++)       //根据N函数值来计算个点
			{
				//P0*N0,3+...P3*N3,3  第一段
				//...
				//P2*N0,3+...P5*N3,3  第三段
				float nVa = b_obj.calcN(i - k, b_obj.degree, b_obj.sepU[j], b_obj.u);
				points += (b_obj.cp[i] * nVa);
			}
			points = points;
			//glVertex3f(points.fX, points.fY, 0.f);
			glVertex2f(points.fX, points.fY);
		}
	}
	glEnd();

}


void myInit()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);//用白色清屏

	glLightfv(GL_LIGHT0, GL_AMBIENT, g_light0_ambient);//设置场景的环境光
	glLightfv(GL_LIGHT0, GL_DIFFUSE, g_light0_diffuse);//设置场景的散射光
	glLightfv(GL_LIGHT0, GL_POSITION, g_light0_position);//设置场景的位置

	glMaterialfv(GL_FRONT, GL_DIFFUSE, g_material);//指定用于光照计算的当前材质属性
	
	glEnable(GL_LIGHTING);//开启灯光
	glEnable(GL_LIGHT0);//开启光照0
	glShadeModel(GL_SMOOTH); //设置着色模式为光滑着色
	glEnable(GL_DEPTH_TEST);//启用深度测试

	glMatrixMode(GL_MODELVIEW); //指定当前矩阵为模型视景矩阵
	glLoadIdentity(); //将当前的用户坐标系的原点移到了屏幕中心：类似于一个复位操作
	gluLookAt(0.0, 0.0, 8.0, 0, 0, 0, 0, 1.0, 0);//该函数定义一个视图矩阵，并与当前矩阵相乘.
	//第一组eyex, eyey,eyez 相机在世界坐标的位置;第二组centerx,centery,centerz 相机镜头对准的物体在世界坐标的位置;第三组upx,upy,upz 相机向上的方向在世界坐标中的方向
}

void loadObjFile(void)
{//加载模型

	//调用系统对话框
	OPENFILENAME  fname; 
	ZeroMemory(&fname, sizeof(fname));
	char strfile[200] = TEXT("*.txt");             
	char szFilter[]=TEXT("文本文档(*.txt)\0");
	fname.lStructSize = sizeof(OPENFILENAME); 
	fname.hwndOwner = NULL; 
	fname.hInstance = NULL; 
	fname.lpstrFilter = szFilter; 
	fname.lpstrCustomFilter = NULL; 
	fname.nFilterIndex = 0; 
	fname.nMaxCustFilter = 0; 
	fname.lpstrFile = strfile; 
	fname.nMaxFile = 200; 
	fname.lpstrFileTitle = NULL; 
	fname.nMaxFileTitle = 0;
	fname.lpstrTitle = NULL; 
	fname.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT; 
	fname.nFileOffset = 0; 
	fname.nFileExtension = 0; 
	fname.lpstrDefExt = 0; 
	fname.lCustData  = NULL; 
	fname.lpfnHook = NULL; 
	fname.lpTemplateName  = NULL;
	fname.lpstrInitialDir = NULL; 
	HDC hDC = wglGetCurrentDC(); 
	HGLRC hRC = wglGetCurrentContext(); 
	GetOpenFileName(&fname); 
	wglMakeCurrent(hDC, hRC); 

	b_obj.readProfile(fname.lpstrFile); //读入配置文件
	b_obj.dpCalcN();
	//glLoadIdentity();
}

void myGlutDisplay() //绘图函数， 操作系统在必要时刻就会对窗体进行重新绘制操作
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); //清除颜色缓冲以及深度缓冲
	glEnable(GL_NORMALIZE); //打开法线向量归一化，确保了法线的长度为1
	
	glMatrixMode(GL_MODELVIEW);//模型视图矩阵
	glPushMatrix(); //压入当前矩阵堆栈

	if (g_draw_content == SHAPE_BSPLINE_DYNAMIC)
	{//TODO 基于动态规划绘制B样条曲线 OK
		DrawBspline(b_obj);
	}
	else if(g_draw_content == SHAPE_BSPLINE_FORMULA)
	{//TODO 基于公式绘制三次B样条曲线 OK
		DrawBspline(20);
	}
	else if (g_draw_content == SHAPE_BSPLINE_RECURSIVE)
	{//TODO 基于递归绘制B样条曲线 OK
		DrawBspline1(b_obj);
	}

	glPopMatrix();
	glutSwapBuffers(); //双缓冲
}

void myGlutReshape(int x,int y) //当改变窗口大小时的回调函数
{
	if (y == 0)
	{
		y = 1;
	}
	
	g_windows_width = x;
	g_windows_height = y;
	double xy_aspect = (float)x / (float)y;
	GLUI_Master.auto_set_viewport(); //自动设置视口大小

	glMatrixMode( GL_PROJECTION );//当前矩阵为投影矩阵
	glLoadIdentity();
	gluPerspective(60.0, xy_aspect, 0.01, 1000.0);//视景体

	glutPostRedisplay(); //标记当前窗口需要重新绘制
}

void myGlutKeyboard(unsigned char Key, int x, int y)
{//键盘时间回调函数

}

void myGlutMouse(int button, int state, int x, int y)
{

}

void myGlutMotion(int x, int y) //处理当鼠标键摁下时,鼠标拖动的事件
{

}

void myGlutIdle(void) //空闲回调函数
{
	if ( glutGetWindow() != g_main_window ) 
		glutSetWindow(g_main_window);  

	glutPostRedisplay();
}

void glui_control(int control ) //处理控件的返回值
{
	switch(control)
	{
	case CRTL_LOAD://选择“open”控件
		loadObjFile();
		g_draw_content = SHAPE_BSPLINE_DYNAMIC;
		break;
	case CRTL_BSPLINE_DYNAMIC:
		g_draw_content = SHAPE_BSPLINE_DYNAMIC;
        break;
	case CRTL_BSPLINE_RECURSIVE:
		g_draw_content = SHAPE_BSPLINE_RECURSIVE;
		break;
	case CRTL_BSPLINE_FORMULA:
		g_draw_content = SHAPE_BSPLINE_FORMULA;
		break;
	default:
		break;
	}
}

void myGlui()
{
	GLUI_Master.set_glutDisplayFunc( myGlutDisplay ); //注册渲染事件回调函数， 系统在需要对窗体进行重新绘制操作时调用
	GLUI_Master.set_glutReshapeFunc( myGlutReshape );  //注册窗口大小改变事件回调函数
	GLUI_Master.set_glutKeyboardFunc( myGlutKeyboard );//注册键盘输入事件回调函数
	glutMotionFunc( myGlutMotion);//注册鼠标移动事件回调函数
	GLUI_Master.set_glutMouseFunc( myGlutMouse );//注册鼠标点击事件回调函数
	GLUI_Master.set_glutIdleFunc(myGlutIdle); //为GLUI注册一个标准的GLUT空闲回调函数，当系统处于空闲时,就会调用该注册的函数

	//GLUI
	GLUI *glui = GLUI_Master.create_glui_subwindow( g_main_window, GLUI_SUBWINDOW_RIGHT); //新建子窗体，位于主窗体的右部 
	new GLUI_StaticText(glui, "GLUI" ); //在GLUI下新建一个静态文本框，输出内容为“GLUI”
	new GLUI_Separator(glui); //新建分隔符
	new GLUI_Button(glui,"Open", CRTL_LOAD, glui_control); //新建按钮控件，参数分别为：所属窗体、名字、ID、回调函数，当按钮被触发时,它会被调用.
	new GLUI_Button(glui, "Quit", 0,(GLUI_Update_CB)exit );//新建退出按钮，当按钮被触发时,退出程序


	GLUI_Panel *draw_panel = glui->add_panel("Draw" ); //在子窗体glui中新建面板，名字为“Draw”
	new GLUI_Button(draw_panel,"Dynamic Bspline", CRTL_BSPLINE_DYNAMIC,glui_control);
	new GLUI_Button(draw_panel,"Recursive Bspline", CRTL_BSPLINE_RECURSIVE,glui_control);
	new GLUI_Button(draw_panel,"Formula Bspline", CRTL_BSPLINE_FORMULA,glui_control);

	glui->set_main_gfx_window(g_main_window ); //将子窗体glui与主窗体main_window绑定，当窗体glui中的控件的值发生过改变，则该glui窗口被重绘
	GLUI_Master.set_glutIdleFunc( myGlutIdle ); 
}

int main(int argc, char* argv[]) //程序入口
{
  /****************************************/
  /*   Initialize GLUT and create window  */
  /****************************************/

  freopen("log.txt", "w", stdout);//重定位，将输出放入log.txt文件中
  glutInit(&argc, argv);//初始化glut
  glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );//初始化渲染模式
  glutInitWindowPosition(200, 200 ); //初始化窗口位置
  glutInitWindowSize(800, 600 ); //初始化窗口大小
 
  g_main_window = glutCreateWindow("Bspline" ); //创建主窗体Model Viewer

  myGlui();
  myInit();

  glutMainLoop();//进入glut消息循环

  return EXIT_SUCCESS;
}

