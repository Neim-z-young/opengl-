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
	void Normalize()//归一化
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
	int degree;                //样条次数 = 阶数 - 1
	int n;                     //控制点数量=阶数
	std::vector<double> u;    //节点矢量 个数 = n+degree+1
	std::vector<Vector2> cp;   //控制点
	std::vector<std::vector<std::vector<double>>> N;   //动态规划时存放基函数序列的数组
	int seg;		//曲线段数
	int k = 20;		//控制点k等分
	std::vector<double> sepU;  //需绘制的点
public:
	//读入样条曲线配置文件
	bool readProfile(const char* pcszFileName);
	// 递归计算基函数值
	double calcN(int i, int k, double t, const std::vector<double> &u);
	//动态规划计算基函数序列
	bool dpCalcN();
};

#endif

