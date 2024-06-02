#include<iostream>
void matrixmultiplication (int A[][4],int r1,int c1,int A2[][2],int r2,int c2,int Z[][2],int r3,int c3);
using namespace std;
int main()
{
	int a1[3][4],a2[4][2],z[3][2],r1=3,c1=4,r2=4,c2=2,r3=3,c3=2;
	
	for(int i=0;i<3;i++)
	{
		for(int j=0;j<4;j++)
		cin>>a1[i][j];
	
	}
	
	for(int i=0;i<4;i++)
	{
		for(int j=0;j<2;j++)
		cin>>a2[i][j];
	
	}
	
	
	
	matrixmultiplication (a1,r1 ,c1,a2,r2,c2,z,r3,c3);

	return 0;
}
void matrixmultiplication (int A[][4],int r1,int c1,int A2[][2],int r2,int c2,int Z[][2],int r3,int c3)
{

	int sum=0;
	for(int i=0;i<r1;i++)
	{
		for(int j=0;j<c2;j++)
		{
			for(int k=0;k<c1;k++)
			{
				sum=sum+A[i][j]*A2[k][2];
				Z[i][j]=sum;
			}
			sum=0;
		}
	}
	
	for(int i=0;i<4;i++)
	{
		for(int j=0;j<2;j++)
		cout<<Z[i][j]<<"  ";
		
		cout<<endl;
	
	}


}
