#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include <gl\glui.h>
#include <math.h>
#include "Obj.h"
#include "common.h"

int g_xform_mode = TRANSFORM_NONE;
int   g_main_window;
double g_windows_width, g_windows_height;

CObj g_obj;
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


void DrawTriangle() 
{//绘制三角形
	glBegin(GL_TRIANGLES);
		glNormal3f(0.0f, 0.0f, 1.0f);  //指定面法向
		glVertex3f( 0.0f, 1.0f, 0.0f);                    // 上顶点
		glVertex3f(-1.0f,-1.0f, 0.0f);                    // 左下
		glVertex3f( 1.0f,-1.0f, 0.0f);                    // 右下
	glEnd();
}

void DrawCube() 
{//绘制立方体
	glBegin(GL_QUADS);  
		glNormal3f( 0.0f, 0.0f, 1.0f);  //指定面法向
		glVertex3f( 1.0f, 1.0f,1.0f);   //列举面顶点数据，逆时针顺序
		glVertex3f(-1.0f, 1.0f, 1.0f);  
		glVertex3f(-1.0f,-1.0f, 1.0f);  
		glVertex3f( 1.0f,-1.0f, 1.0f);  
	//前----------------------------  
		glNormal3f( 0.0f, 0.0f,-1.0f);  
		glVertex3f(-1.0f,-1.0f,-1.0f);  
		glVertex3f(-1.0f, 1.0f,-1.0f);  
		glVertex3f( 1.0f, 1.0f,-1.0f);  
		glVertex3f( 1.0f,-1.0f,-1.0f);  
	//后----------------------------  
		glNormal3f( 0.0f, 1.0f, 0.0f);  
		glVertex3f( 1.0f, 1.0f, 1.0f);  
		glVertex3f( 1.0f, 1.0f,-1.0f);  
		glVertex3f(-1.0f, 1.0f,-1.0f);  
		glVertex3f(-1.0f, 1.0f, 1.0f);  
	//上----------------------------  
		glNormal3f( 0.0f,-1.0f, 0.0f);  
		glVertex3f(-1.0f,-1.0f,-1.0f);  
		glVertex3f( 1.0f,-1.0f,-1.0f);  
		glVertex3f( 1.0f,-1.0f, 1.0f);  
		glVertex3f(-1.0f,-1.0f, 1.0f);  
	//下----------------------------  
	glNormal3f( 1.0f, 0.0f, 0.0f);  
		glVertex3f( 1.0f, 1.0f, 1.0f);  
		glVertex3f( 1.0f,-1.0f, 1.0f);  
		glVertex3f( 1.0f,-1.0f,-1.0f);  
		glVertex3f( 1.0f, 1.0f,-1.0f);  
	//右----------------------------  
	glNormal3f(-1.0f, 0.0f, 0.0f);  
		glVertex3f(-1.0f,-1.0f,-1.0f);  
		glVertex3f(-1.0f,-1.0f, 1.0f);  
		glVertex3f(-1.0f, 1.0f, 1.0f);  
		glVertex3f(-1.0f, 1.0f,-1.0f);  
	//左----------------------------*/  
	glEnd();  
}

void DrawCircle() 
{//绘制圆
	glBegin(GL_POLYGON);
		glNormal3f(0.0f, 0.0f, 1.0f);
		for(int i=0; i<n; ++i)
			glVertex2f(R*cos(2*Pi/n*i), R*sin(2*Pi/n*i));
	glEnd();
}

GLvoid DrawCircleArea(float cx, float cy, float cz, float r, int num_segments)
{//绘制圆
	glBegin(GL_POLYGON);
	glNormal3f(cx, cy, 1.0f);
	for (int i = 0; i < num_segments; ++i)
		glVertex3f(r*cos(2 * Pi / num_segments * i), r*sin(2 * Pi / num_segments * i), cz);
	glEnd();
}

void DrawCylinder()
{//绘制圆柱
	GLUquadricObj *g_text;
	g_text = gluNewQuadric();
	float topRadius = 0.4, baseRadius = 0.4, height = 1.5, segment = 128, stacks = 4;
	gluCylinder(g_text, baseRadius, topRadius, height, segment, stacks);
	//top
	DrawCircleArea(0.f, 0.f, height, topRadius, segment);
	//base
	DrawCircleArea(0.f, 0.f, 0.f, baseRadius, segment);
}

void DrawCone()
{//绘制圆锥
	GLUquadricObj *g_text;
	g_text = gluNewQuadric();
	float topRadius = 0.4f, baseRadius = 0.f, height = 1.5, segment = 128, stacks = 4;
	gluCylinder(g_text, baseRadius, topRadius, height, segment, stacks);
	//top
	DrawCircleArea(0.f, 0.f, height, topRadius, segment);
	//base
	DrawCircleArea(0.f, 0.f, 0.f, baseRadius, segment);
}


void DrawModel(CObj &model)
{//TODO: 绘制模型  OK
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < model.m_faces.size(); i++)
	{
		glNormal3f(model.m_faces[i].normal.fX, model.m_faces[i].normal.fY, model.m_faces[i].normal.fZ);
		int p0 = model.m_faces[i].pts[0] - 1;
		int p1 = model.m_faces[i].pts[1] - 1;
		int p2 = model.m_faces[i].pts[2] - 1;
		glVertex3f(model.m_pts[p0].normal.fX, model.m_pts[p0].normal.fY, model.m_pts[p0].normal.fZ);
		glVertex3f(model.m_pts[p1].normal.fX, model.m_pts[p1].normal.fY, model.m_pts[p1].normal.fZ);
		glVertex3f(model.m_pts[p2].normal.fX, model.m_pts[p2].normal.fY, model.m_pts[p2].normal.fZ);
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
	char strfile[200] = TEXT("*.obj");             
	char szFilter[]=TEXT("OBJ Files(*.OBJ)\0");
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

	g_obj.ReadObjFile(fname.lpstrFile); //读入模型文件
	//各种参数归零
	g_x_offset = 0.0;
	g_y_offset = 0.0;
	g_z_offset = 0.0;
	g_scale_size = 1;
}

void myGlutDisplay() //绘图函数， 操作系统在必要时刻就会对窗体进行重新绘制操作
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); //清除颜色缓冲以及深度缓冲
	glEnable(GL_NORMALIZE); //打开法线向量归一化，确保了法线的长度为1
	
	glMatrixMode(GL_MODELVIEW);//模型视图矩阵
	glPushMatrix(); //压入当前矩阵堆栈


	if (g_draw_content == SHAPE_MODEL)
	{//绘制模型
		//glLoadIdentity();
		DrawModel(g_obj);
	}
	else if (g_draw_content == SHAPE_TRIANGLE)  //画三角形
	{
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -6.0f);  
		DrawTriangle();
	}
	else if(g_draw_content == SHAPE_CUBE)  //画立方体
	{
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -6.0f);  
		glRotatef(g_rquad, g_rquad, g_rquad, 1.0f);	// 在XYZ轴上旋转立方体
		DrawCube();
		g_rquad+=0.2f;// 增加旋转变量
	}
	else if (g_draw_content == SHAPE_CIRCLE) // 画圆
	{
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -6.0f);  
		DrawCircle();
	}
	else if (g_draw_content == SHAPE_CYLINDER)  
	{//TODO: 添加画圆柱的代码   OK
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -6.0f);
		glRotatef(g_rquad, g_rquad, g_rquad, 1.0f);	// 在XYZ轴上旋转圆柱
		DrawCylinder();
		g_rquad += 0.2f;// 增加旋转变量
	}
	else if (g_draw_content == SHAPE_CONE) 
	{//TODO：添加画圆锥的代码   OK
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -6.0f);
		glRotatef(g_rquad, g_rquad, g_rquad, 1.0f);	// 在XYZ轴上旋转圆锥
		DrawCone();
		g_rquad += 0.2f;// 增加旋转变量
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
	if (state == GLUT_DOWN) //鼠标的状态为按下
	{
		g_press_x = x;
		g_press_y = y; 
		if (button == GLUT_LEFT_BUTTON) 
		{//按下鼠标的左键表示对模型进行旋转操作
			g_xform_mode = TRANSFORM_ROTATE;
		}
		else if (button == GLUT_MIDDLE_BUTTON)  
		{//按下鼠标的滚轮表示对模型进行平移操作
			g_xform_mode = TRANSFORM_TRANSLATE; 
		}
		else if (button ==  GLUT_RIGHT_BUTTON) 
		{//按下鼠标的右键表示对模型进行缩放操作
			g_xform_mode = TRANSFORM_SCALE; 
		}
	}
	else if (state == GLUT_UP)  
	{//如果没有按鼠标，则不对模型进行任何操作
		g_xform_mode = TRANSFORM_NONE; 
	}
}

void myGlutMotion(int x, int y) //处理当鼠标键摁下时,鼠标拖动的事件
{
	const int shift_step = 10;
	int shift_x = x - g_press_x, shift_y = y - g_press_y;
	g_press_x = x;
	g_press_y = y;
	if (g_xform_mode == TRANSFORM_ROTATE) //旋转
	{//TODO:添加鼠标移动控制模型旋转参数的代码   左键旋转OK

		glRotatef(9.f, shift_y, shift_x, 0.0f);	// 在XYZ轴上旋转模型
	}
	else if(g_xform_mode == TRANSFORM_SCALE) //缩放
	{//TODO:添加鼠标移动控制模型缩放参数的代码   右键缩放OK
		if (shift_x > shift_step)
		{
			g_scale_size = 1.2;
		}
		else if(shift_x < -shift_step)
		{
			g_scale_size = 0.83;
		}
		glScalef(g_scale_size, g_scale_size, g_scale_size);
	}
	else if(g_xform_mode == TRANSFORM_TRANSLATE) //平移
	{//TODO:添加鼠标移动控制模型平移参数的代码   中键平移OK
		if (shift_x > shift_step)
			g_x_offset = 0.2;
		else if (shift_x < -shift_step)
			g_x_offset = -0.2;
		else
			g_x_offset = 0;
		if (shift_y > shift_step)
			g_y_offset = -0.2;
		else if (shift_y < -shift_step)
			g_y_offset = 0.2;
		else
			g_y_offset = 0;
		glTranslatef(g_x_offset, g_y_offset, 0.0f);
	}

	// force the redraw function
	glutPostRedisplay(); 
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
		g_draw_content = SHAPE_MODEL;
		break;
	case CRTL_CHANGE://选择Type面板
		if (g_view_type == VIEW_POINT)  
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); // 设置两面均为顶点绘制方式
		}
		else if (g_view_type == VIEW_WIRE)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // 设置两面均为线段绘制方式
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // 设置两面为填充方式
		}	
		break;
	case CRTL_TRIANGLE:
		g_draw_content = SHAPE_TRIANGLE;
        break;
	case CRTL_CUBE:
		g_draw_content = SHAPE_CUBE;
		break;
	case CRTL_CIRCLE:
		g_draw_content = SHAPE_CIRCLE;
		break;
	case CRTL_CYLINDER:
		g_draw_content = SHAPE_CYLINDER;
		break;
	case CRTL_CONE:
		g_draw_content = SHAPE_CONE;
		break;
	case CRTL_MODEL:
		g_draw_content = SHAPE_MODEL;
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

	GLUI_Panel *type_panel = glui->add_panel("Type" ); //在子窗体glui中新建面板，名字为“Type”
	GLUI_RadioGroup *radio = glui->add_radiogroup_to_panel(type_panel, &g_view_type, CRTL_CHANGE, glui_control); //在Type面板中添加一组单选按钮
	glui->add_radiobutton_to_group(radio, "points"); 
	glui->add_radiobutton_to_group(radio, "wire");
	glui->add_radiobutton_to_group(radio, "flat");

	GLUI_Panel *draw_panel = glui->add_panel("Draw" ); //在子窗体glui中新建面板，名字为“Draw”
	new GLUI_Button(draw_panel,"Triangle",CRTL_TRIANGLE,glui_control);
	new GLUI_Button(draw_panel,"Cube",CRTL_CUBE,glui_control);
	new GLUI_Button(draw_panel,"Circle",CRTL_CIRCLE,glui_control);
	new GLUI_Button(draw_panel,"Cylinder",CRTL_CYLINDER,glui_control);
	new GLUI_Button(draw_panel,"Cone",CRTL_CONE,glui_control);
	new GLUI_Button(draw_panel,"Model",CRTL_MODEL,glui_control);

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
 
  g_main_window = glutCreateWindow("Model Viewer" ); //创建主窗体Model Viewer

  myGlui();
  myInit();

  glutMainLoop();//进入glut消息循环

  return EXIT_SUCCESS;
}

