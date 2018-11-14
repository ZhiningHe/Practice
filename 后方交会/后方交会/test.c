#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<math.h>

typedef struct point_3D point_3D;
typedef struct point_2D point_2D;

struct point_3D  //放置三维坐标
{
	float x;
	float y;
	float z;
}Land[4];//XYZ
struct point_2D  //放置二维坐标
{
	float x;
	float y;
}Photo[4];//xy


double * MatrixInver(double A[], int m, int n) //求转置
{
	int i, j;
	double *B = NULL;
	B = (double *)malloc(m*n*sizeof(double));
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < m; j++)
		{
			B[i*m + j] = A[j*n + i];
		}
	}
	return B;
}

double * MatrixOpp(double A[][], int m, int n)//求逆
{
	int i,j,x, y, k;
	double *SP = NULL, *AB = NULL, *B = NULL,X,*C;
	X = Surplus(A, m, n);
	X = 1/X;
	X = 1 / X;
	for (i = 0; i <m; i ++)
	for (j = 0; j < n; j++)
	{
		for (k = 0; k < m*n; k++)
		{
			B[k] = A[k];

			for (x = 0; x < n; x++)
			{
				B[i*n + x] = 0;
				for (y = 0; y < m; y++)
					B[m*y + j] = 0;
				B[i*n + j] = 1;
				SP[i*n + j] = Surplus(B, m, n);
				AB[i*n + j] = X*SP[i*n + j];
			}
		}
		C = MatrixInver(AB, m, n);
		return C;
	}
}

int main()
{
	sqrt(0);
	float m = 0.00002, H = 0.0;//比例尺、航高
	point_3D P0 = {0,0,0.15324};//内方位元素x0、y0、f
	point_3D S;//摄影中心Xs、Ys、Zs
	point_3D angle = {0,0,0};//外方位元素，三个旋转角度
	float R[3][3] = { 0 };//旋转矩阵

	printf("输入4个相片点坐标xy：\n");		//输入坐标
	for (int i = 0; i < 4; i++)
	{
		scanf("%f %f", &Photo[i].x,&Photo[i].y);
	}
	printf("输入4个地面点坐标XYZ：\n");
	for (int i = 0; i < 4; i++)
	{
		scanf("%f %f", &Land[i].x, &Land[i].y, &Land[i].z);
	}
	
	float sum_x = 0.0, sum_y = 0.0, sum_z = 0.0;	//计算初始值
	for (int i = 0; i < 4; i++)
	{
		sum_x += Land[i].x;
		sum_y += Land[i].y;
		sum_z += Land[i].z;
	}
	S.x = sum_x / 4;
	S.y = sum_y / 4;
	S.z = sum_z / 4;

	int tl = 0;
	do
	{
		//旋转矩阵
		R[0][0] = cos(angle.x)*cos(angle.z) - sin(angle.x)*sin(angle.y)*sin(angle.z);
		R[0][1] = -cos(angle.x)*sin(angle.z)-sin(angle.x)*sin(angle.y)*sin(angle.z);
		R[0][2] = -sin(angle.x)*cos(angle.y);
		R[1][0] = cos(angle.y)*sin(angle.z);
		R[1][1] = cos(angle.y)*cos(angle.z);
		R[1][2] = -sin(angle.y);
		R[2][0] = sin(angle.x)*cos(angle.z) - cos(angle.x)*sin(angle.y)*sin(angle.z);
		R[2][1] = -sin(angle.x)*sin(angle.z) - cos(angle.x)*sin(angle.y)*sin(angle.z);
		R[2][2] = cos(angle.x)*cos(angle.y);

		float a1 = R[0][0], b1 = R[1][0], c1 = R[2][0];
		float a2 = R[0][1], b2 = R[1][1], c2 = R[2][1];
		float a3 = R[0][2], b3 = R[1][2], c3 = R[2][2];

		point_2D _p_[4];	//近似值(x)(y)
		point_2D L[4];		//改正数l
		float A[2][6];		//误差方程系数矩阵
		point_3D P[4];		//求A矩阵的中间数
		point_2D p;			//误差方程
		float X[9];			//外方位元素改正值

		for (int i = 0; i<4; i++)
		{
			_p_[i].x = -(P0.z)*(a1 * (Land[i].x - S.x) + b1 * (Land[i].y - S.y) + c1 * (Land[i].z - S.z)) / (a3*(Land[i].x - S.x) + b3*(Land[i].y - S.y) + c3*(Land[i].z - S.z));
			_p_[i].y = -(P0.z)*(a2 * (Land[i].x - S.x) + b2 * (Land[i].y - S.y) + c2 * (Land[i].z - S.z)) / (a3*(Land[i].x - S.x) + b3*(Land[i].y - S.y) + c3*(Land[i].z - S.z));
			L[i].x = Photo[i].x - _p_[i].x;
			L[i].y = Photo[i].y - _p_[i].y;		//改正数L
			P[i].x = a1*(Land[i].x - S.x) + b1*(Land[i].y - S.y) + c1* (Land[i].z - S.z);
			P[i].y = a2*(Land[i].x - S.x) + b2*(Land[i].y - S.y) + c2* (Land[i].z - S.z);
			P[i].z = a3*(Land[i].x - S.x) + b3*(Land[i].y - S.y) + c3* (Land[i].z - S.z);
			p.x = (-P0.z*P[i].x) / P[i].z;
			p.y = (-P0.z*P[i].y) / P[i].z;		//共线方程

			A[0][0] = (a1*P0.z + a3*p.x) / P[i].z;//a11
			A[0][1] = (b1*P0.z + b3*p.x) / P[i].z;//a12
			A[0][3] = (c1*P0.z + c3*p.x) / P[i].z;//a13
			A[1][0] = (b2*P0.z + b3*p.y) / P[i].z;//a21
			A[1][1] = (b2*P0.z + b3*p.y) / P[i].z;//a22
			A[1][2] = (b2*P0.z + b3*p.y) / P[i].z;//a23
			A[0][3] = p.y*sin(angle.y)-(p.x * (p.x*cos(angle.z)-p.y*sin(angle.z))/ P0.z + P0.z * cos(angle.z))*cos(angle.y);		//a14
			A[0][4] = (-P0.z*sin(angle.z)) - (p.x*(p.x*sin(angle.z) + p.y*cos(angle.z))) / P0.z;//a15
			A[0][5] = p.y;		//a16
			A[1][3] = -p.x*sin(angle.y) - (p.y * (p.x*cos(angle.z) - p.y*sin(angle.z)) / P0.z + P0.z * sin(angle.z))*cos(angle.y);		//a24
			A[1][4] = (-P0.z*cos(angle.z)) - (p.y*(p.x*sin(angle.z) + p.y*cos(angle.z))) / P0.z;//a25
			A[1][5] = p.x;		//a26
			float a11 = A[0][0], a12 = A[0][1], a13 = A[0][2], a14 = A[0][3], a15 = A[0][4], a16 = A[0][5];
			float a21 = A[1][0], a22 = A[1][1], a23 = A[1][2], a24 = A[1][3], a25 = A[1][4], a26 = A[1][5];		//系数矩阵A
			//X=(A*A1)^-1*A1*L
			double A1[6][2] = { 0 }, A2[6][6] = { 0 }; double* A3[6][6] = { 0 };
			for (int i = 0; i < 6; i++)
			{
				for (int j = 0; j < 2; j++)
				{
					for (int k = 0; k < 6; k++)
					{
						A2[i][k] += A1[i][j] * A[j][k];
					}
				}
			}
			A3[6][6] = MatrixOpp(A4[6][2], 6, 2);
			for (int i = 0; i < 6; i++)
			{
				for (int j = 0; j < 2; j++)
				{
					for (int k = 0; k < 6; k++)
					{
						X[i][k] += A1[i][j] * L[j][k];
					}
				}
			}
	} while (tl > 0.1);
		S.x += X[0][0];//外方位元素
		S.y += X[0][1];
		S.z += X[0][2];
		angle.x += X[0][3];
		angle.y += X[0][4];
		angle.z += X[0][5];
		P0.x += X[0][6];
		P0.y += X[0][7];
		P0.z += X[0][8];

	system("pause");
	return 0;
}