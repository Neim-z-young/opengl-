#include "bObj.h"
#include <iostream>
#include <sstream>
#include <algorithm>
using std::min;
using std::max;

Vector2 operator + (const Vector2& one, const Vector2& two) //�����������
{
	return Vector2(one.fX + two.fX, one.fY + two.fY);
}

Vector2 operator - (const Vector2& one, const Vector2& two) //�����������
{
	return Vector2(one.fX - two.fX, one.fY - two.fY);
}

Vector2 operator * (const Vector2& one, double scale) //���������ĳ˲���
{
	return Vector2(one.fX * scale, one.fY * scale);
}

Vector2 operator / (const Vector2& one, double scale) //���������ĳ�����
{
	return one * (1.0 / scale);
}

bool Bspline::readProfile(const char * pcszFileName)
{
	FILE* fpFile = fopen(pcszFileName, "r"); //��ֻ����ʽ���ļ�
	if (fpFile == NULL)
	{
		return false;
	}

	u.clear();
	cp.clear();
	N.clear();

	//TODO���������ļ��е����ݽ��м���  OK
	char buff[255];
	int temp;
	double tempd, tempx, tempy;
	fscanf(fpFile, "%d", &degree);
	fscanf(fpFile, "%d", &n);
	for (int i = 0; i < n + degree + 1; i++)
	{
		fscanf(fpFile, "%lf", &tempd);
		u.push_back(tempd);
	}
	//int max = u[degree + n];
	//for (int i = 0; i < n; i++)
	//{
	//	u[i] /= max;
	//}

	while (!feof(fpFile))
	{
		fscanf(fpFile, "%lf", &tempx);
		fscanf(fpFile, "%lf", &tempy);
		cp.push_back(Vector2(tempx, tempy));
	}

	seg = cp.size() - n + 1;

	//���Ƶ�֮�仮��k��
	std::vector<double> sepU(u.size() * k, 0.f);
	double delta = 0.1;
	delta = (u[u.size() - 1] - u[0]+1) / sepU.size();
	for (int i = 0; i < sepU.size(); i++)
	{
		sepU[i] = u[0] + i * delta;
	}
	this->sepU = sepU;

	//ά�ȣ������Ƶ���������+1������ʸ���ֶΣ�
	std::vector<std::vector<std::vector<double>>> t(u.size()-1, std::vector<std::vector<double>>(degree+1, std::vector<double>(sepU.size(), 0.f)));
	N = t;
	fclose(fpFile);

	return true;
}

double Bspline::calcN(int i, int k, double t, const std::vector<double>& u)
{
	if (k == 0)
	{
		if (t >= u[i] && t <= u[i + 1])
		{
			return 1.f;
		}
		else
		{
			return 0.f;
		}
	}
	else
	{
		double length1, length2;
		length1 = u[i + k] - u[i];
		length2 = u[i + k+1] - u[i + 1];
		// ����0/0=0
		if (length1 <= EPSILON && length1 >= -EPSILON)
		{
			length1 = 1.f;
		}
		if (length2 <= EPSILON && length2 >= -EPSILON)
		{
			length2 = 1.f;
		}
		float baseN = calcN(i, k - 1, t, u)*(t - u[i]) / length1 +
			calcN(i + 1, k - 1, t, u)*(u[i + k + 1] - t) / length2;
		return baseN;
	}
}

bool Bspline::dpCalcN()
{
	//��ʼ��������
	for (int i = 0; i < N.size(); i++)
	{
		for (int t = 0; t < N[i][0].size(); t++)
		{
			if (sepU[t] >= u[i] && sepU[t] <= u[i + 1])
			{
				N[i][0][t] = 1.f;
			}
		}
	}
	if (N.size() == 0) return false;
	//���õ��ƹ�ʽ
	for (int k = 1; k < N[0].size(); k++)  //���ݵ��ƹ�ʽ֪���״����ڵ�ѭ�������
	{
		for (int i = 0; i < N.size(); i++)
		{
			if (i + k > u.size()-2) break;   //����
			for (int t = 0; t < N[i][k].size(); t++)
			{
				double length1, length2;
				length1 = u[i + k] - u[i];
				length2 = u[i + k + 1] - u[i + 1];
				// ����0/0=0
				if (length1 <= EPSILON && length1 >= -EPSILON)
				{
					length1 = 1.f;
				}
				if (length2 <= EPSILON && length2 >= -EPSILON)
				{
					length2 = 1.f;
				}
				N[i][k][t] = (sepU[t] - u[i]) / length1 * N[i][k - 1][t] + 
					(u[i + k + 1] - sepU[t]) / length2 * N[i + 1][k - 1][t];
			}
		}
	}
	return true;
}
