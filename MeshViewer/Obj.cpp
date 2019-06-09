#include "Obj.h"
#include <iostream>
#include <sstream>
#include <algorithm>

using std::min;
using std::max;

Vector3 operator + (const Vector3& one, const Vector3& two) //两个向量相加
{
	return Vector3(one.fX + two.fX, one.fY + two.fY, one.fZ + two.fZ);
}

Vector3 operator - (const Vector3& one, const Vector3& two) //两个向量相减
{
	return Vector3(one.fX - two.fX, one.fY - two.fY, one.fZ - two.fZ);
}

Vector3 operator * (const Vector3& one, double scale) //向量与数的乘操作
{
	return Vector3(one.fX * scale, one.fY * scale, one.fZ * scale);
}

Vector3 operator / (const Vector3& one, double scale) //向量与数的除操作
{
	return one * (1.0 / scale);
}

Vector3 Cross(Vector3& one, Vector3& two) 
{//计算两个向量的叉积
	Vector3 vCross;								

	vCross.fX = ((one.fY * two.fZ) - (one.fZ * two.fY));
	vCross.fY = ((one.fZ * two.fX) - (one.fX * two.fZ));
	vCross.fZ = ((one.fX * two.fY) - (one.fY * two.fX));

	return vCross;		
}

CObj::CObj(void)
{
}


CObj::~CObj(void)
{
}

bool CObj::ReadObjFile(const char* pcszFileName) 
{//读取模型文件

	FILE* fpFile = fopen(pcszFileName, "r"); //以只读方式打开文件
	if (fpFile == NULL)
	{
		return false;
	}

	m_pts.clear(); 
	m_faces.clear();

	//TODO：将模型文件中的点和面数据分别存入m_pts和m_faces中
	char buff[255];
	char temp;
	while (!feof(fpFile))
	{
		fscanf(fpFile, "%c", &temp);
		if (temp == 'v')
		{
			Vector3 tempPoints;
			fscanf(fpFile, "%lf", &tempPoints.fX);
			fscanf(fpFile, "%lf", &tempPoints.fY);
			fscanf(fpFile, "%lf", &tempPoints.fZ);
			Point point;
			point.pos = tempPoints;
			point.normal = tempPoints;
			m_pts.push_back(point);
			fscanf(fpFile, "%c", &temp);
		}
		else if (temp == 'f')
		{ 
			Face face;
			fscanf(fpFile, "%d", &face.pts[0]);
			fscanf(fpFile, "%d", &face.pts[1]);
			fscanf(fpFile, "%d", &face.pts[2]);
			m_faces.push_back(face);
			fscanf(fpFile, "%c", &temp);
		}
		else if (temp == '\n') //跳过空行
		{

		}
		else
		{
			fgets(buff, 255, fpFile);
		}
	}
	
	fclose(fpFile);

	UnifyModel(); //将模型归一化

	return true;
}

void CObj::UnifyModel()
{//为统一显示不同尺寸的模型，将模型归一化，将模型尺寸缩放到0.0-1.0之间
//原理：找出模型的边界最大和最小值，进而找出模型的中心
//以模型的中心点为基准对模型顶点进行缩放
//TODO:添加模型归一化代码

}

void CObj::ComputeFaceNormal(Face& f)
{//TODO:计算面f的法向量，并保存
	Vector3 x1 = m_pts[f.pts[1]-1].pos - m_pts[f.pts[0]-1].pos;
	Vector3 x2 = m_pts[f.pts[2]-1].pos - m_pts[f.pts[0]-1].pos;
	Vector3 temp = Cross(x1, x2);
	temp.Normalize();
	f.normal = temp;
}