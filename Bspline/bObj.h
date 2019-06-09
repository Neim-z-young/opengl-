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


//3
//4
//0 0.5 1 2 3 3.5 4.5 5
//0 0
//10 50
//30 10
//100 100
class Bspline
{
public:
	int degree;                //����
	int n;                     //���Ƶ�����
	std::vector<double> u;    //�ڵ�ʸ��
	std::vector<Vector2> cp;   //���Ƶ�
	std::vector<std::vector<std::vector<double>>> N;
	int seg; //���߶���
	std::vector<double> sepU;
public:
	bool readProfile(const char* pcszFileName); //�����������������ļ�
	double calcN(int i, int k, double t, const std::vector<double> &u); //�ݹ����Nֵ 
	bool dpCalcN();
};

#endif

