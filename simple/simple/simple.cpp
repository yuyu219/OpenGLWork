#include <GL/glut.h>
#include <cstdio>
#include <GL/freeglut.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <Windows.h>
#include <string>
#include <sstream>
#include <mmsystem.h>
#include <GL/glut.h>
using namespace std;

#pragma comment(lib, "glew32.lib")

void init();
void mydisplay();
void Keyboard(int, int, int); //glutSpecialFunc
//void Keyboard(unsinged char key, int x, int y)  //glutKeyboardFunc
void MouseWhere(int, int, int, int);
//void MotionMouse(int, int);

void ball_display(float, float, float, float, int);
void angle_dispaly();
void score_dispaly();
void ballnum_dispaly();

void reshape(int, int);

#define ori_y -340  //起點y
#define PI  3.1415926
#define G  0.98     //重力加速度:0.98 cm/(ms*ms)

struct ball {
	double x, y, z, r;

	double fly_x = 0, r_yz = 0, fly_y = 0, fly_z = 0, angle = 0; //球的初速度
	double fly = 0, visibal = 0; //球顯示

	int board_touch = 0, pillar_touch = 0, basket_touch = 0, squre_touch = 0, floor_touch = 0, falltime = 0; //碰撞判斷用
	int out = 0;
	int add_score = 0;
};

ball bas[10];
int i = 0, m = 0;
float lookat = -340;   //視角
float mouse_move = 0, mousedown_x = 0, mousedown_y = 0, mousemove_x = 0, mousemove_y = 0;
float before_x = 0, before_y = 0, before_z = 0;  //判斷反射線用

//障礙物
//1.地板
float y = -340;
//2.籃板
float board_x = -90, board_y = -105, board_z = -460, board_length = 180, board_width = 105; //左上為點
//3.紅線
float boardr_x = -29.5, boardr_y = -150, boardr_z = -460, boardr_length = 59, boardr_width = 45; //左上為點
//3.柱子
float pillar_x = 0, pillar_y = -160, pillar_z = -580 - 10, pillar_heigh = 340, pillar_r = 10;  //中上為點
float pillar2_x = 0, pillar2_y = -160, pillar2_z = -580 - 20, pillar2_heigh = 140, pillar2_r = 10;  //中上為點
//4.籃框
float basket_x = 0, basket_y = -195, basket_z = -415.5, basket_inner_r = 27.5, basket_outer_r = 29.5;  //中心為點
//5.籃框支架
float squre_x = -7.5, squre_y = -195, squre_z = -458, squre_length = 15, squre_width = 13;

//subwindows
int main_w, angle_w, score_w, ballnum_w;

//計算分數用
int score = 0;

//debug用
float max_y = -500;

void OPENGL_DRIVER(int *argc, char **argv) {
	glutInit(argc, argv);
}
int main(int argc, char** argv) {
	OPENGL_DRIVER(&argc, argv);
	glutInitWindowSize(1000, 1000);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	main_w = glutCreateWindow("simple");

	init();
	glutMouseFunc(MouseWhere);
	//glutMotionFunc(MotionMouse);
	glutSpecialFunc(Keyboard);
	//glutKeyboardFunc(Keyboard);
	glutDisplayFunc(mydisplay);

	glutReshapeFunc(reshape); //重繪子視窗

	for (m = 0; m < 10; m++)
	{
		bas[m].x = 0;
		bas[m].y = -320;
		bas[m].z = -20;
		bas[m].r = 12.3;
	}

	//subwindows
	angle_w = glutCreateSubWindow(main_w, 780, 20, 200, 150);  //角度的子視窗
	glutDisplayFunc(angle_dispaly);

	score_w = glutCreateSubWindow(main_w, 20, 20, 80, 40);  //分數的子視窗
	glutDisplayFunc(score_dispaly);

	ballnum_w = glutCreateSubWindow(main_w, 20, 70, 200, 40);  //球數的子視窗
	glutDisplayFunc(ballnum_dispaly);

	glutMainLoop();
	return 0;
}

void init()
{
	glMatrixMode(GL_PROJECTION);
	gluPerspective(110, 1, 0.00001, 5000);
	//gluLookAt(-750, 150, -290, 0, 150, -290, 0, 1, 0); //從左側看

	//glOrtho(-20.0, 20.0, -20.0, 20.0, -20.0, 20.0);
	//glOrtho(-750.0, 750.0, -400.0, 400.0, 0.0, 1000.0); //籃框
	//glFrustum(-750.0, 750.0, -200.0, 400.0, 100.0, 1000.0); //籃框
	//glFrustum(-320.0, 320.0, -240.0, 240.0, 400, 1000);

	glMatrixMode(GL_MODELVIEW);
}

//滑鼠點擊
void MouseWhere(int btn, int state, int x, int y)
{
	if (btn == GLUT_LEFT_BUTTON && i < 10)
	{
		if (state == GLUT_DOWN)
		{
			if (i == 0) //第一顆
			{
				bas[i].visibal = 1;
				mousedown_x = x;
				mousedown_y = y;
				glutPostRedisplay();
			}
			else if (bas[i - 1].visibal == 0) //不是第一顆
			{
				bas[i].visibal = 1;
				mousedown_x = x;
				mousedown_y = y;
				glutPostRedisplay();
			}
			else  //沒球
			{
				cout << "no ball" << endl;
			}
		}
		else if (state == GLUT_UP)
		{
			if (bas[i].visibal == 1)
			{
				bas[i].fly = 1;
				bas[i].fly_x = -(x - mousedown_x) / 30;
				bas[i].r_yz = y - mousedown_y;

				bas[i].fly_y = bas[i].r_yz * sinf(bas[i].angle / 180 * PI) / 30;
				bas[i].fly_z = bas[i].r_yz * cosf(bas[i].angle / 180 * PI) / 30;
				cout << "初速度: " << bas[i].fly_x << ", " << bas[i].fly_y << ", " << bas[i].fly_z << endl;
				cout << "yz初速度: " << bas[i].r_yz << ", 角度: " << bas[i].angle << endl;

				/*//initialize
				float length = sqrtf(bas[j].fly_x*bas[j].fly_x + bas[j].fly_y*bas[j].fly_y + bas[j].fly_z*bas[j].fly_z);
				bas[j].fly_x = bas[j].fly_x / length;
				bas[j].fly_y = bas[j].fly_y / length;
				bas[j].fly_z = bas[j].fly_z / length;
				cout << bas[j].fly_x << " " << bas[j].fly_y << " " << bas[j].fly_z << " " << r_yz << " " << angle << " " << sinf(angle / 180 * PI) << endl;
				*/
				if (bas[i].fly_z < 0)
				{
					bas[i].fly_x = 0;
					bas[i].fly_y = 0;
					bas[i].fly_z = 0;
				}

				i++;
				bas[i].angle = bas[i - 1].angle;
				glutPostWindowRedisplay(ballnum_w);
			}
		}
	}
}

void Keyboard(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		if (bas[i].angle < 90)
		{
			bas[i].angle += 5;
			lookat = ori_y + sinf(bas[i].angle / 360 * PI);
			if (bas[i].fly == 0)
			{
				glutPostWindowRedisplay(main_w);
				glutPostWindowRedisplay(angle_w);
			}
			else
			{
				glutPostWindowRedisplay(angle_w);
			}
		}
		break;
	case GLUT_KEY_DOWN:
		if (bas[i].angle > -90)
		{
			bas[i].angle -= 5;
			lookat = ori_y + sinf(bas[i].angle / 360 * PI);
			if (bas[i].fly == 0)
			{
				glutPostWindowRedisplay(main_w);
				glutPostWindowRedisplay(angle_w);
			}
			else
			{
				glutPostWindowRedisplay(angle_w);
			}
		}
		break;
	default:
		break;
	}

}

void ball_display(float x, float y, float z, float r, int j)
{
	//球
	glPushMatrix();
	glColor4f(1.0, 0.0, 0.0, 1.0);
	glTranslatef(x, y, z);
	Sleep(20);
	glutSolidSphere(r, 40, 40);
	glPopMatrix();

	if (bas[j].fly == 1)
	{
		if (bas[j].y > max_y)
		{
			max_y = bas[j].y;
		}
		if (bas[j].x > 750 + 100 || bas[j].x < -750 - 100 || bas[j].z > 0 || bas[j].z < -2000 || bas[j].falltime > 3)
		{
			bas[j].visibal = 0;
			bas[j].fly = 0;

			//加分
			if (bas[j].add_score != 0)
			{
				score += bas[j].add_score;
				cout << "第" << j + 1 << "球得分: " << bas[j].add_score << endl;
				bas[j].add_score = 0;

				glutPostWindowRedisplay(score_w);
			}

			cout << endl;
		}

		before_x = bas[j].x;
		before_y = bas[j].y;
		before_z = bas[j].z;

		bas[j].x = bas[j].x + bas[j].fly_x;
		bas[j].y = bas[j].y + bas[j].fly_y;
		bas[j].z = bas[j].z - bas[j].fly_z;

		bas[j].fly_y = bas[j].fly_y - G / 2;

		//離開籃板後
		if (bas[j].z <= -460 && bas[j].out == 0)
		{
			bas[j].out = 1;
			//cout << bas[j].x << "\t" << bas[j].y << "\t" << bas[j].z << endl;
		}
		//地面
		if (bas[j].y <= -500)
		{
			bas[j].fly_y = -bas[j].fly_y;
			bas[j].falltime++;

			//PlaySound(TEXT("pa.wav"), NULL, SND_SYNC | SND_FILENAME);
			if (bas[j].floor_touch == 0)
			{
				bas[j].fly_y -= 2;  //摩擦力?!
				bas[j].floor_touch = 1;
			}

			//加分
			if (bas[j].falltime == 1)
			{
				score += bas[j].add_score;
				cout << "第" << j + 1 << "球得分: " << bas[j].add_score << endl;
				bas[j].add_score = 0;

				glutPostWindowRedisplay(score_w);
			}
		}
		else
		{
			bas[j].floor_touch = 0;
		}
		//籃板
		if (abs(bas[j].z - (-460)) <= bas[j].r)
		{
			if ((bas[j].x >= -board_length / 2 && bas[j].x <= board_length / 2
				&& bas[j].y <= board_y && bas[j].y >= board_y - board_width) && bas[j].board_touch == 0)  //籃板範圍內
			{
				bas[j].fly_z = -bas[j].fly_z;

				bas[j].board_touch = 1;

				if ((bas[j].x >= -boardr_length / 2 && bas[j].x <= boardr_length / 2
					&& bas[j].y <= boardr_y && bas[j].y >= boardr_y - boardr_width))  //紅線內
				{
					cout << "碰到板子紅線內" << endl;
					//cout << "球心位置: " << bas[j].x << ", " << bas[j].y << ", " << bas[j].z << endl;
					if (bas[j].add_score < 10 && bas[j].falltime == 0)
					{
						bas[j].add_score = 10;
					}
				}
				else
				{
					cout << "碰到板子" << endl;
					//cout << "球心位置: " << bas[j].x << ", " << bas[j].y << ", " << bas[j].z << endl;
					if (bas[j].add_score < 5 && bas[j].falltime == 0)
					{
						bas[j].add_score = 5;
					}
				}
			}
		}
		//碰到圓柱
		if (sqrtf((bas[j].z - pillar_z)*(bas[j].z - pillar_z) + (bas[j].x - pillar_x)*(bas[j].x - pillar_x)) <= bas[j].r + pillar_r)
		{
			if (bas[j].y <= -210 && bas[j].y >= -500 && bas[j].pillar_touch == 0)
			{
				cout << "碰到柱子" << endl;
				//cout << "球心位置: " << bas[j].x << ", " << bas[j].y << ", " << bas[j].z << endl;
				float IdotN = (bas[j].x - before_x) * (bas[j].x - pillar_x)
					+ (bas[j].y - before_y) * 0
					+ (bas[j].z - before_z) * (bas[j].z - pillar_z);
				bas[j].fly_x = (bas[j].x - before_x) - 2 * IdotN * (bas[j].x - pillar_x);
				bas[j].fly_y = (bas[j].y - before_y) - 2 * IdotN * 0;
				bas[j].fly_z = (bas[j].z - before_z) - 2 * IdotN * (bas[j].z - pillar_z);
				float length = sqrtf(bas[j].fly_x*bas[j].fly_x + bas[j].fly_y*bas[j].fly_y + bas[j].fly_z*bas[j].fly_z);
				bas[j].fly_x = bas[j].fly_x / length * 10;
				bas[j].fly_y = bas[j].fly_y / length * 10;
				bas[j].fly_z = bas[j].fly_z / length * 10;

				if (bas[j].add_score < 1 && bas[j].falltime == 0)
				{
					bas[j].add_score = 1;
				}
				bas[j].pillar_touch = 1;
			}
		}
		//籃框支架??
		if ((bas[j].y - bas[j].r <= -195 && bas[j].x <= squre_x + squre_length && bas[j].x >= squre_x
			&& bas[j].z <= squre_z && bas[j].z >= squre_z + squre_width) && bas[j].squre_touch == 0)
		{
			cout << "碰到籃框支架" << endl;
			//cout << "球心位置: " << bas[j].x << ", " << bas[j].y << ", " << bas[j].z << endl;
			float IdotN = (bas[j].x - before_x) * (bas[j].x - 0)
				+ (bas[j].y - before_y) * (bas[j].y - 1)
				+ (bas[j].z - before_z) * (bas[j].z - 0);
			bas[j].fly_x = (bas[j].x - before_x) - 2 * IdotN * (bas[j].x - 0);
			bas[j].fly_y = (bas[j].y - before_y) - 2 * IdotN * (bas[j].y - 1);
			bas[j].fly_z = (bas[j].z - before_z) - 2 * IdotN * (bas[j].z - 0);
			float length = sqrtf(bas[j].fly_x*bas[j].fly_x + bas[j].fly_y*bas[j].fly_y + bas[j].fly_z*bas[j].fly_z);
			bas[j].fly_x = bas[j].fly_x / length * 10;
			bas[j].fly_y = bas[j].fly_y / length * 10;
			bas[j].fly_z = bas[j].fly_z / length * 10;

			if (bas[j].falltime == 0 && bas[j].add_score < 30)
			{
				bas[j].add_score = 30;
			}
			bas[j].squre_touch = 1;
		}
		//籃框
		if ((bas[j].y - bas[j].r <= basket_y && bas[j].y > basket_y
			&& bas[j].x >= basket_x - basket_outer_r && bas[j].x <= basket_x + basket_outer_r
			&& bas[j].z >= basket_z - basket_outer_r && bas[j].z <= basket_z + basket_outer_r) && bas[j].basket_touch == 0)
		{
			float r_bas_basket = sqrtf((bas[j].x - basket_x)*(bas[j].x - basket_x)
				+ (bas[j].y - basket_y)*(bas[j].y - basket_y)
				+ (bas[j].z - basket_z)*(bas[j].z - basket_z)); //兩心距離
			float xz_bas_basket = sqrtf((bas[j].x - basket_x)*(bas[j].x - basket_x)
				+ (bas[j].z - basket_z)*(bas[j].z - basket_z)); //不看y的xz距離

			if (r_bas_basket <= basket_inner_r - bas[j].r) //入籃
			{
				cout << "進球" << endl;
				//cout << bas[j].x << "\t" << bas[j].y << "\t" << bas[j].z << endl;
				if (bas[j].add_score < 100 && bas[j].falltime == 0)
				{
					bas[j].add_score = 100;
				}
			}
			if (xz_bas_basket < basket_inner_r && xz_bas_basket > basket_inner_r - bas[j].r && bas[j].y - basket_y <= bas[j].r) //打籃框內側
			{
				cout << "碰到籃框內側" << endl;
				//cout << bas[j].x << "\t" << bas[j].y << "\t" << bas[j].z << endl;
				float IdotN = (bas[j].x - before_x) * (basket_x - bas[j].x)
					+ (bas[j].y - before_y) * 0
					+ (bas[j].z - before_z) * (basket_z - bas[j].z);
				bas[j].fly_x = (bas[j].x - before_x) - 2 * IdotN * (basket_x - bas[j].x);
				bas[j].fly_y = (bas[j].y - before_y) - 2 * IdotN * 0;
				bas[j].fly_z = (bas[j].z - before_z) - 2 * IdotN * (basket_z - bas[j].z);
				float length = sqrtf(bas[j].fly_x*bas[j].fly_x + bas[j].fly_y*bas[j].fly_y + bas[j].fly_z*bas[j].fly_z);
				bas[j].fly_x = bas[j].fly_x / length * 10;
				bas[j].fly_y = bas[j].fly_y / length * 10;
				bas[j].fly_z = bas[j].fly_z / length * 10;
				//cout << "\t" << bas[j].x << "\t" << bas[j].y << "\t" << bas[j].z << endl;
				//cout << bas[j].x - before_x << "\t" << bas[j].y - before_y << "\t" << bas[j].z - before_z << endl;
				//cout << bas[j].fly_x << "\t" << bas[j].fly_y << "\t" << bas[j].fly_z << endl;
				if (bas[j].add_score < 50 && bas[j].falltime == 0)
				{
					bas[j].add_score = 50;
				}
			}
			if (xz_bas_basket >= basket_inner_r && xz_bas_basket <= basket_outer_r && bas[j].y - basket_y <= bas[j].r) //打籃框上側
			{
				cout << "碰到籃框上側" << endl;
				//cout << bas[j].x << "\t" << bas[j].y << "\t" << bas[j].z << endl;
				float IdotN = (bas[j].x - before_x) * 0
					+ (bas[j].y - before_y) * (bas[j].y - basket_y)
					+ (bas[j].z - before_z) * 0;
				bas[j].fly_x = (bas[j].x - before_x) - 2 * IdotN * 0;
				bas[j].fly_y = (bas[j].y - before_y) - 2 * IdotN * (bas[j].y - basket_y);
				bas[j].fly_z = (bas[j].z - before_z) - 2 * IdotN * 0;
				float length = sqrtf(bas[j].fly_x*bas[j].fly_x + bas[j].fly_y*bas[j].fly_y + bas[j].fly_z*bas[j].fly_z);
				bas[j].fly_x = bas[j].fly_x / length * 10;
				bas[j].fly_y = bas[j].fly_y / length * 10;
				bas[j].fly_z = bas[j].fly_z / length * 10;
				//cout << "\t" << bas[j].x << "\t" << bas[j].y << "\t" << bas[j].z << endl;
				//cout << bas[j].x - before_x << "\t" << bas[j].y - before_y << "\t" << bas[j].z - before_z << endl;
				//cout << bas[j].fly_x << "\t" << bas[j].fly_y << "\t" << bas[j].fly_z << endl;
				if (bas[j].add_score < 50 && bas[j].falltime == 0)
				{
					bas[j].add_score = 50;
				}
			}
			if (xz_bas_basket >= basket_outer_r && xz_bas_basket <= basket_outer_r + bas[j].r && bas[j].y - basket_y <= bas[j].r) //打籃框外側
			{
				cout << "碰到籃框外側" << endl;
				//cout << bas[j].x << "\t" << bas[j].y << "\t" << bas[j].z << endl;
				float IdotN = (bas[j].x - before_x) * 0
					+ (bas[j].y - before_y) * (bas[j].y - basket_y)
					+ (bas[j].z - before_z) * 0;
				bas[j].fly_x = (bas[j].x - before_x) - 2 * IdotN * 0;
				bas[j].fly_y = (bas[j].y - before_y) - 2 * IdotN * (bas[j].y - basket_y);
				bas[j].fly_z = (bas[j].z - before_z) - 2 * IdotN * 0;
				float length = sqrtf(bas[j].fly_x*bas[j].fly_x + bas[j].fly_y*bas[j].fly_y + bas[j].fly_z*bas[j].fly_z);
				bas[j].fly_x = bas[j].fly_x / length * 10;
				bas[j].fly_y = bas[j].fly_y / length * 10;
				bas[j].fly_z = bas[j].fly_z / length * 10;
				//cout << "\t" << bas[j].x << "\t" << bas[j].y << "\t" << bas[j].z << endl;
				//cout << bas[j].x - before_x << "\t" << bas[j].y - before_y << "\t" << bas[j].z - before_z << endl;
				//cout << bas[j].fly_x << "\t" << bas[j].fly_y << "\t" << bas[j].fly_z << endl;
				if (bas[j].add_score < 50 && bas[j].falltime == 0)
				{
					bas[j].add_score = 50;
				}
			}

			bas[j].basket_touch = 1;
		}
	}
}

void mydisplay()
{
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	//gluLookAt(0.0, 0.0, -460.0, 0.0, -340.0, -461.0, 0.0, 1.0, 0.0); //籃板上方
	//gluLookAt(500.0, -160.0, -400.0, 0.0, -160.0, -460.0, 0.0, 1.0, 0.0); //籃板側面
	gluLookAt(0.0, -340.0, 0.0, 0.0, lookat, -1.0, 0.0, 1.0, 0.0);

	//球場
	glPushMatrix();
	glColor4f(1.0, 1.0, 1.0, 1.0);
	GLfloat curSizeLine = 5;
	glLineWidth(curSizeLine);
	glBegin(GL_LINES);
	glVertex3f(750.0, -500.0, -580.0);
	glVertex3f(750.0, -500.0, 580.0);
	glVertex3f(-750.0, -500.0, 580.0);
	glVertex3f(-750.0, -500.0, -580.0);
	glVertex3f(-755.0, -500.0, -580.0);
	glVertex3f(755.0, -500.0, -580.0);
	glEnd();
	glPopMatrix();

	//罰球線
	glPushMatrix();
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glBegin(GL_QUADS);
	glVertex3f(300.0, -500.0, -580.0);
	glVertex3f(-300.0, -500.0, -580.0);
	glVertex3f(-180.0, -500.0, 0.0);
	glVertex3f(180.0, -500.0, 0.0);
	glEnd();
	glPopMatrix();

	//柱子
	glPushMatrix();
	glColor4f(1.0, 1.0, 1.0, 1.0);
	GLUquadricObj *quadricObj = gluNewQuadric();
	gluQuadricDrawStyle(quadricObj, GLU_LINE);
	glTranslatef(0, pillar_y, pillar_z);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(quadricObj, pillar_r, pillar_r, pillar_heigh, 40, 40);
	glPopMatrix();

	glPushMatrix();
	glColor4f(1.0, 1.0, 1.0, 1.0);
	gluQuadricDrawStyle(quadricObj, GLU_LINE);
	glTranslatef(0, pillar2_y, pillar2_z);
	gluCylinder(quadricObj, pillar2_r, pillar2_r, pillar2_heigh, 40, 40);
	glPopMatrix();

	//籃板
	glPushMatrix();
	glColor4f(1.0, 1.0, 1.0, 0.0);
	glBegin(GL_QUADS);
	glVertex3f(board_x, board_y, board_z);
	glVertex3f(board_x + board_length, board_y, board_z);
	glVertex3f(board_x + board_length, board_y - board_width, board_z);
	glVertex3f(board_x, board_y - board_width, board_z);
	glEnd();
	glPopMatrix();

	//紅線
	glPushMatrix();
	glColor4f(1.0, 0.0, 0.0, 0.0);
	glLineWidth(curSizeLine);
	glBegin(GL_LINES);
	glVertex3f(boardr_x + boardr_length, boardr_y - boardr_width, board_z); //右下
	glVertex3f(boardr_x - 2.5, boardr_y - boardr_width, board_z); //左下

	glVertex3f(boardr_x, boardr_y - boardr_width, board_z); //左下
	glVertex3f(boardr_x, boardr_y + 2.5, board_z); //左上

	glVertex3f(boardr_x, boardr_y, board_z); //左上
	glVertex3f(boardr_x + boardr_length + 2.5, boardr_y, board_z); //右上

	glVertex3f(boardr_x + boardr_length, boardr_y, board_z);  //右上
	glVertex3f(boardr_x + boardr_length, boardr_y - boardr_width - 2.5, board_z); //右下
	glEnd();
	glPopMatrix();

	//籃框支架
	glPushMatrix();
	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_QUADS);
	glVertex3f(squre_x, squre_y, squre_z + squre_width);
	glVertex3f(squre_x + squre_length, squre_y, squre_z + squre_width);
	glVertex3f(squre_x + squre_length, squre_y, squre_z);
	glVertex3f(squre_x, squre_y, squre_z);
	glEnd();
	glPopMatrix();

	//籃框
	glPushMatrix();
	glColor3f(0.0, 1.0, 0.0);
	glTranslatef(basket_x, basket_y, basket_z);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	gluDisk(quadricObj, basket_inner_r, basket_outer_r, 40, 40);
	glPopMatrix();

	for (m = 0; m < 10; m++)
	{
		if (bas[m].visibal == 1)
		{
			ball_display(bas[m].x, bas[m].y, bas[m].z, bas[m].r, m);
		}
	}
	glutPostRedisplay();


	glFlush();
	glutSwapBuffers();
}

void angle_dispaly()
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	//基準線
	glPushMatrix();
	glColor3f(1, 0, 0);
	GLfloat curSizeLine = 5;
	glLineWidth(curSizeLine);
	glBegin(GL_LINES);
	glVertex2f(-0.7, 0);
	glVertex2f(0.7, 0);
	glEnd();
	glPopMatrix();

	//視角
	glPushMatrix();
	glColor3f(0, 1, 0);
	glLineWidth(curSizeLine);
	glBegin(GL_LINES);
	glVertex2f(0.7 - 1.4*cosf(bas[i].angle / 180 * PI), 1.4*sinf(bas[i].angle / 180 * PI));
	glVertex2f(0.7, 0);
	glEnd();
	glPopMatrix();

	glFlush();
	glutSwapBuffers();
}

void score_dispaly()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glPushMatrix();
	glLoadIdentity();
	glColor3f(1.0, 1.0, 1.0);
	glRasterPos2f(-0.2, 0);

	stringstream ss;
	string s;
	ss << score;
	s = ss.str();
	for (int n = 0; n < s.length(); n++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, s[n]);
	}

	glFlush();
	glutSwapBuffers();
}

void ballnum_dispaly()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glPushMatrix();
	glLoadIdentity();
	glColor3f(1.0, 1.0, 1.0);
	glRasterPos2f(-0.7, 0);

	stringstream ss;
	string s = "";

	ss << 10 - i;
	s = "You have " + ss.str() + " ball.";
	for (int n = 0; n < s.length(); n++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, s[n]);
	}
	glPopMatrix();

	glFlush();
	glutSwapBuffers();
}

void reshape(int w, int h)
{
	glutReshapeWindow(1000, 1000);
}