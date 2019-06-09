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
static GLfloat g_light0_ambient[] =  {0.0f, 0.0f, 0.0f, 1.0f};//������
static GLfloat g_light0_diffuse[] =  {1.0f, 1.0f, 1.0f, 1.0f};//ɢ���
static GLfloat g_light0_specular[] = {1.0f,1.0f,1.0f,1.0f}; //�����
static GLfloat g_light0_position[] = {0.0f, 0.0f, 100.0f, 0.0f};//��Դ��λ�á���4������Ϊ1����ʾ���Դ����4��������Ϊ0����ʾƽ�й���{0.0f, 0.0f, 10.0f, 0.0f}

static GLfloat g_material[] = {0.96f, 0.8f, 0.69f, 1.0f};//����
static GLfloat g_rquad = 0;

static float g_x_offset   = 0.0;
static float g_y_offset   = 0.0;
static float g_z_offset   = 0.0;
static float g_scale_size = 1; 
static int  g_press_x; //��갴��ʱ��x����
static int  g_press_y; //��갴��ʱ��y����

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

/*ȫ�ֱ���*/
Point2 vec[NUM_POINTS];
bool mouseLeftDown = false;


/*����B��������*/
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


void DrawBspline(Bspline &b_obj)
{
	glLineWidth(2.f);
	glBegin(GL_LINE_STRIP);
	//glNormal3f(0.0f, 0.0f, 1.0f);  //ָ���淨��
	for (int i = 0; i < b_obj.cp.size(); i++)
	{
		//glVertex3f(b_obj.cp[i].fX, b_obj.cp[i].fY, 0.f);
		glVertex2f(b_obj.cp[i].fX, b_obj.cp[i].fY);
	}
	glEnd();

	glPointSize(10.0f);
	glBegin(GL_POINTS);
	//glNormal3f(0.0f, 0.0f, 1.0f);  //ָ���淨��
	for (int i = 0; i < b_obj.cp.size(); i++)
	{
		//glVertex3f(b_obj.cp[i].fX, b_obj.cp[i].fY, 0.f);
		glVertex2f(b_obj.cp[i].fX, b_obj.cp[i].fY);
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
//	glNormal3f(0.0f, 0.0f, 1.0f);  //ָ���淨��
	for (int k = 0; k < b_obj.seg; k++)
	{
		for (int j = 0; j < b_obj.sepU.size(); j++) {
			Vector2 points(0, 0);
			for (int i = k; i < k+b_obj.n; i++)
			{
				//points += (b_obj.cp[i] * b_obj.calcN(i, b_obj.degree, b_obj.u[j], b_obj.u));
				points += (b_obj.cp[i] * b_obj.N[i][b_obj.degree][j]);
			}
			points = points * b_obj.degree;
			//glVertex3f(points.fX, points.fY, 0.f);
			glVertex2f(points.fX, points.fY);
		}
	}
	glEnd();

	glPointSize(4.0f);
	glBegin(GL_POINTS);
	//	glNormal3f(0.0f, 0.0f, 1.0f);  //ָ���淨��
	for (int k = 0; k < b_obj.seg; k++)
	{
		for (int j = 0; j < b_obj.sepU.size(); j++) {
			Vector2 points(0, 0);
			for (int i = k; i < k + b_obj.n; i++)
			{
				//points += (b_obj.cp[i] * b_obj.calcN(i, b_obj.degree, b_obj.u[j], b_obj.u));
				points += (b_obj.cp[i] * b_obj.N[i][b_obj.degree][j]);
			}
			points = points * b_obj.degree;
			//glVertex3f(points.fX, points.fY, 0.f);
			glVertex2f(points.fX, points.fY);
		}
	}
	glEnd();
}

void DrawTriangle()
{//����������
	glBegin(GL_TRIANGLES);
	glNormal3f(0.0f, 0.0f, 1.0f);  //ָ���淨��
	glVertex3f(0.0f, 1.0f, 0.0f);                    // �϶���
	glVertex3f(-1.0f, -1.0f, 0.0f);                    // ����
	glVertex3f(1.0f, -1.0f, 0.0f);                    // ����
	glEnd();
}

void myInit()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);//�ð�ɫ����

	glLightfv(GL_LIGHT0, GL_AMBIENT, g_light0_ambient);//���ó����Ļ�����
	glLightfv(GL_LIGHT0, GL_DIFFUSE, g_light0_diffuse);//���ó�����ɢ���
	glLightfv(GL_LIGHT0, GL_POSITION, g_light0_position);//���ó�����λ��

	glMaterialfv(GL_FRONT, GL_DIFFUSE, g_material);//ָ�����ڹ��ռ���ĵ�ǰ��������
	
	glEnable(GL_LIGHTING);//�����ƹ�
	glEnable(GL_LIGHT0);//��������0
	glShadeModel(GL_SMOOTH); //������ɫģʽΪ�⻬��ɫ
	glEnable(GL_DEPTH_TEST);//������Ȳ���

	glMatrixMode(GL_MODELVIEW); //ָ����ǰ����Ϊģ���Ӿ�����
	glLoadIdentity(); //����ǰ���û�����ϵ��ԭ���Ƶ�����Ļ���ģ�������һ����λ����
	gluLookAt(0.0, 0.0, 8.0, 0, 0, 0, 0, 1.0, 0);//�ú�������һ����ͼ���󣬲��뵱ǰ�������.
	//��һ��eyex, eyey,eyez ��������������λ��;�ڶ���centerx,centery,centerz �����ͷ��׼�����������������λ��;������upx,upy,upz ������ϵķ��������������еķ���
}

void loadObjFile(void)
{//����ģ��

	//����ϵͳ�Ի���
	OPENFILENAME  fname; 
	ZeroMemory(&fname, sizeof(fname));
	char strfile[200] = TEXT("*.txt");             
	char szFilter[]=TEXT("�ı��ĵ�(*.txt)\0");
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

	b_obj.readProfile(fname.lpstrFile); //���������ļ�
	b_obj.dpCalcN();
	//glLoadIdentity();
}

void myGlutDisplay() //��ͼ������ ����ϵͳ�ڱ�Ҫʱ�̾ͻ�Դ���������»��Ʋ���
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); //�����ɫ�����Լ���Ȼ���
	glEnable(GL_NORMALIZE); //�򿪷���������һ����ȷ���˷��ߵĳ���Ϊ1
	
	glMatrixMode(GL_MODELVIEW);//ģ����ͼ����
	glPushMatrix(); //ѹ�뵱ǰ�����ջ


	if (g_draw_content == SHAPE_MODEL)
	{//����ģ��
		//glLoadIdentity();
		//glTranslatef(0.0f, -0.0f, -9.0f);
		DrawBspline(b_obj);
	}
	else if (g_draw_content == SHAPE_TRIANGLE)  //��������
	{
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -6.0f);  
		DrawTriangle();
	}
	else if(g_draw_content == SHAPE_CUBE)  //��������
	{
		/*glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -6.0f);  */
		DrawBspline(20);

	}
	else if (g_draw_content == SHAPE_CIRCLE) // ��Բ
	{
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -6.0f);  
	}
	else if (g_draw_content == SHAPE_CYLINDER)  
	{//TODO: ��ӻ�Բ���Ĵ���

	}
	else if (g_draw_content == SHAPE_CONE) 
	{//TODO����ӻ�Բ׶�Ĵ���

	}
	glPopMatrix();
	glutSwapBuffers(); //˫����
}

void myGlutReshape(int x,int y) //���ı䴰�ڴ�Сʱ�Ļص�����
{
	if (y == 0)
	{
		y = 1;
	}
	
	g_windows_width = x;
	g_windows_height = y;
	double xy_aspect = (float)x / (float)y;
	GLUI_Master.auto_set_viewport(); //�Զ������ӿڴ�С

	glMatrixMode( GL_PROJECTION );//��ǰ����ΪͶӰ����
	glLoadIdentity();
	gluPerspective(60.0, xy_aspect, 0.01, 1000.0);//�Ӿ���

	glutPostRedisplay(); //��ǵ�ǰ������Ҫ���»���
}

void myGlutKeyboard(unsigned char Key, int x, int y)
{//����ʱ��ص�����

}

void myGlutMouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN) //����״̬Ϊ����
	{
		g_press_x = x;
		g_press_y = y; 
		printf("%d   %d\n", x, y);
		if (button == GLUT_LEFT_BUTTON) 
		{//�������������ʾ��ģ�ͽ�����ת����
			g_xform_mode = TRANSFORM_ROTATE;
		}
		else if (button == GLUT_MIDDLE_BUTTON)  
		{//�������Ĺ��ֱ�ʾ��ģ�ͽ���ƽ�Ʋ���
			g_xform_mode = TRANSFORM_TRANSLATE; 
		}
		else if (button ==  GLUT_RIGHT_BUTTON) 
		{//���������Ҽ���ʾ��ģ�ͽ������Ų���
			g_xform_mode = TRANSFORM_SCALE; 
		}
	}
	else if (state == GLUT_UP)  
	{//���û�а���꣬�򲻶�ģ�ͽ����κβ���
		g_xform_mode = TRANSFORM_NONE; 
	}
}

void myGlutMotion(int x, int y) //������������ʱ,����϶����¼�
{
	const int shift_step = 10;
	int shift_x = x - g_press_x, shift_y = y - g_press_y;
	g_press_x = x;
	g_press_y = y;
	if (g_xform_mode == TRANSFORM_ROTATE) //��ת
	{//TODO:�������ƶ�����ģ����ת�����Ĵ���

		glRotatef(9.f, shift_x, shift_y, 0.0f);	// ��XYZ������תģ��
	}
	else if(g_xform_mode == TRANSFORM_SCALE) //����
	{//TODO:�������ƶ�����ģ�����Ų����Ĵ���
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
	else if(g_xform_mode == TRANSFORM_TRANSLATE) //ƽ��
	{//TODO:�������ƶ�����ģ��ƽ�Ʋ����Ĵ���
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

void myGlutIdle(void) //���лص�����
{
	if ( glutGetWindow() != g_main_window ) 
		glutSetWindow(g_main_window);  

	glutPostRedisplay();
}

void glui_control(int control ) //����ؼ��ķ���ֵ
{
	switch(control)
	{
	case CRTL_LOAD://ѡ��open���ؼ�
		loadObjFile();
		g_draw_content = SHAPE_MODEL;
		break;
	case CRTL_CHANGE://ѡ��Type���
		if (g_view_type == VIEW_POINT)  
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); // ���������Ϊ������Ʒ�ʽ
		}
		else if (g_view_type == VIEW_WIRE)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // ���������Ϊ�߶λ��Ʒ�ʽ
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // ��������Ϊ��䷽ʽ
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
	GLUI_Master.set_glutDisplayFunc( myGlutDisplay ); //ע����Ⱦ�¼��ص������� ϵͳ����Ҫ�Դ���������»��Ʋ���ʱ����
	GLUI_Master.set_glutReshapeFunc( myGlutReshape );  //ע�ᴰ�ڴ�С�ı��¼��ص�����
	GLUI_Master.set_glutKeyboardFunc( myGlutKeyboard );//ע����������¼��ص�����
	glutMotionFunc( myGlutMotion);//ע������ƶ��¼��ص�����
	GLUI_Master.set_glutMouseFunc( myGlutMouse );//ע��������¼��ص�����
	GLUI_Master.set_glutIdleFunc(myGlutIdle); //ΪGLUIע��һ����׼��GLUT���лص���������ϵͳ���ڿ���ʱ,�ͻ���ø�ע��ĺ���

	//GLUI
	GLUI *glui = GLUI_Master.create_glui_subwindow( g_main_window, GLUI_SUBWINDOW_RIGHT); //�½��Ӵ��壬λ����������Ҳ� 
	new GLUI_StaticText(glui, "GLUI" ); //��GLUI���½�һ����̬�ı����������Ϊ��GLUI��
	new GLUI_Separator(glui); //�½��ָ���
	new GLUI_Button(glui,"Open", CRTL_LOAD, glui_control); //�½���ť�ؼ��������ֱ�Ϊ���������塢���֡�ID���ص�����������ť������ʱ,���ᱻ����.
	new GLUI_Button(glui, "Quit", 0,(GLUI_Update_CB)exit );//�½��˳���ť������ť������ʱ,�˳�����

	GLUI_Panel *type_panel = glui->add_panel("Type" ); //���Ӵ���glui���½���壬����Ϊ��Type��
	GLUI_RadioGroup *radio = glui->add_radiogroup_to_panel(type_panel, &g_view_type, CRTL_CHANGE, glui_control); //��Type��������һ�鵥ѡ��ť
	glui->add_radiobutton_to_group(radio, "points"); 
	glui->add_radiobutton_to_group(radio, "wire");
	glui->add_radiobutton_to_group(radio, "flat");

	GLUI_Panel *draw_panel = glui->add_panel("Draw" ); //���Ӵ���glui���½���壬����Ϊ��Draw��
	new GLUI_Button(draw_panel,"Triangle",CRTL_TRIANGLE,glui_control);
	new GLUI_Button(draw_panel,"Cube",CRTL_CUBE,glui_control);
	new GLUI_Button(draw_panel,"Circle",CRTL_CIRCLE,glui_control);
	new GLUI_Button(draw_panel,"Cylinder",CRTL_CYLINDER,glui_control);
	new GLUI_Button(draw_panel,"Cone",CRTL_CONE,glui_control);
	new GLUI_Button(draw_panel,"Model",CRTL_MODEL,glui_control);

	glui->set_main_gfx_window(g_main_window ); //���Ӵ���glui��������main_window�󶨣�������glui�еĿؼ���ֵ�������ı䣬���glui���ڱ��ػ�
	GLUI_Master.set_glutIdleFunc( myGlutIdle ); 
}

int main(int argc, char* argv[]) //�������
{
  /****************************************/
  /*   Initialize GLUT and create window  */
  /****************************************/

  freopen("log.txt", "w", stdout);//�ض�λ�����������log.txt�ļ���
  glutInit(&argc, argv);//��ʼ��glut
  glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );//��ʼ����Ⱦģʽ
  glutInitWindowPosition(200, 200 ); //��ʼ������λ��
  glutInitWindowSize(800, 600 ); //��ʼ�����ڴ�С
 
  g_main_window = glutCreateWindow("Model Viewer" ); //����������Model Viewer

  myGlui();
  myInit();

  glutMainLoop();//����glut��Ϣѭ��

  return EXIT_SUCCESS;
}

