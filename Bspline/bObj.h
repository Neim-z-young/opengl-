#ifndef OBJ_CLASS
#define OBJ_CLASS

#include <vector>
#include <cmath>

#define EPSILON  (1e-6)

struct Vector2;
Vector2 operator + (const Vector2& one, const Vector2& two);
Vector2 operator - (const Vector2& one, const Vector2& two);
Vector2 operator * (const Vector2& one, double scale);
Vector2 operator / (const Vector2& one, double scale);

struct Vector2
{
	double fX;
	double fY;
	Vector2(double x = 0.0, double y = 0.0) : fX(x), fY(y) {}
	Vector2 operator +=(const Vector2& v) { return *this = *this + v; }
	double Length() { return sqrt(fX * fX + fY * fY); }
	void Normalize()//��һ��
	{
		double fLen = Length();
		if (fabs(fLen) > 1e-6)
		{
			fX /= fLen;
			fY /= fLen;
		}
	}
};


class Bspline
{
public:
	int degree;                //�������� = ���� - 1
	int n;                     //���Ƶ�����=����
	std::vector<double> u;    //�ڵ�ʸ�� ���� = n+degree+1
	std::vector<Vector2> cp;   //���Ƶ�
	std::vector<std::vector<std::vector<double>>> N;   //��̬�滮ʱ��Ż��������е�����
	int seg;		//���߶���
	int k = 20;		//���Ƶ�k�ȷ�
	std::vector<double> sepU;  //����Ƶĵ�
public:
	//�����������������ļ�
	bool readProfile(const char* pcszFileName);
	// �ݹ���������ֵ
	double calcN(int i, int k, double t, const std::vector<double> &u);
	//��̬�滮�������������
	bool dpCalcN();
};

#endif

