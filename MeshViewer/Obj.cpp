#include "Obj.h"
#include <iostream>
#include <sstream>
#include <algorithm>

using std::min;
using std::max;

Vector3 operator + (const Vector3& one, const Vector3& two) //�����������
{
	return Vector3(one.fX + two.fX, one.fY + two.fY, one.fZ + two.fZ);
}

Vector3 operator - (const Vector3& one, const Vector3& two) //�����������
{
	return Vector3(one.fX - two.fX, one.fY - two.fY, one.fZ - two.fZ);
}

Vector3 operator * (const Vector3& one, double scale) //���������ĳ˲���
{
	return Vector3(one.fX * scale, one.fY * scale, one.fZ * scale);
}

Vector3 operator / (const Vector3& one, double scale) //���������ĳ�����
{
	return one * (1.0 / scale);
}

Vector3 Cross(Vector3& one, Vector3& two) 
{//�������������Ĳ��
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
{//��ȡģ���ļ�

	FILE* fpFile = fopen(pcszFileName, "r"); //��ֻ����ʽ���ļ�
	if (fpFile == NULL)
	{
		return false;
	}

	m_pts.clear(); 
	m_faces.clear();

	//TODO����ģ���ļ��еĵ�������ݷֱ����m_pts��m_faces��
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
		else if (temp == '\n') //��������
		{

		}
		else
		{
			fgets(buff, 255, fpFile);
		}
	}
	
	fclose(fpFile);

	UnifyModel(); //��ģ�͹�һ��

	return true;
}

void CObj::UnifyModel()
{//Ϊͳһ��ʾ��ͬ�ߴ��ģ�ͣ���ģ�͹�һ������ģ�ͳߴ����ŵ�0.0-1.0֮��
//ԭ���ҳ�ģ�͵ı߽�������Сֵ�������ҳ�ģ�͵�����
//��ģ�͵����ĵ�Ϊ��׼��ģ�Ͷ����������
//TODO:���ģ�͹�һ������

}

void CObj::ComputeFaceNormal(Face& f)
{//TODO:������f�ķ�������������
	Vector3 x1 = m_pts[f.pts[1]-1].pos - m_pts[f.pts[0]-1].pos;
	Vector3 x2 = m_pts[f.pts[2]-1].pos - m_pts[f.pts[0]-1].pos;
	Vector3 temp = Cross(x1, x2);
	temp.Normalize();
	f.normal = temp;
}