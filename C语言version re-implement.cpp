#include <io.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <fstream>
#include <cv.h>
#include<time.h>
#include <highgui.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
//#include <basic_structure.cpp>
//#include <nls_C.h>
using namespace cv;
using namespace std;

//structure define area;
typedef struct ___POINT___{
	int x;
	int y;
} POINT;

//线段起点、终点
typedef struct  __UIMG2DotLine__
{
	POINT	ptSt;
	POINT	ptEn;
}UIMG2DotLine;

typedef unsigned char BYTE;
typedef unsigned long		u32;
typedef struct tagUFORM_IMAGEPARAM
{
	BYTE* pSrc;
	BYTE* pTag;
	u32   nWidth;
	u32   nHeight;
}uFORM_ImgParam;
// Global variables

//
int nl=0,nc=0;
double rate=0.028;
int threshold_for_count=200;
int threshold_value2=230;
int costtime;
int threshold_value_re=150;

int thresh_detect_image =160;

int min_45distance=20; //need to be verified
FILE *fp_Write;
FILE *fp_Read;
clock_t start;
clock_t end_time;
Mat src, src_gray,after_threshold;
uFORM_ImgParam Form_Src,Form_Gray,After_Threshold,After_Callback;
POINT up_point,down_point,left_point,right_point,cross_point;
/// Function headers
float powf(float x,float y);
double abs(double x);
double fabs(double x);
double sqrt(double x);

void count_noise_point(int, void*);
uFORM_ImgParam ConvImg2FormImg(Mat image);
uFORM_ImgParam Conv2GrayFormImg(Mat Image);
int verify(Mat image,uFORM_ImgParam Form_Image);
void gray_process(Mat src);
void assign_mat(Mat temp_mat,uFORM_ImgParam Form_Image);
void threshold_implement(uFORM_ImgParam Form_Image);
void threshold_re(uFORM_ImgParam Form_Image);
BYTE* check(int j,int i,uFORM_ImgParam Form_Image);
void thresh_callback_implement(uFORM_ImgParam Form_Image);
void assign_point(int x,int y,POINT* temp);
void thresh_callback2_implement(uFORM_ImgParam Form_Image);
double distance_re(POINT A,POINT B);
int thresh_callback_check(void);
void draw_circle(void);
void show_4point_data(void);
void thresh_callback_re(uFORM_ImgParam Form_Image);
void CrossPoint(const UIMG2DotLine *Line1, const UIMG2DotLine *Line2);
void find_cross_point(uFORM_ImgParam Form_Image);
void record_data(char* imageName);


int main( int argc, char** argv )
{
	char readfile[]="D:\\internship\\test\\LIST.TXT";
	char writefile[]="D:\\internship\\test\\test3.txt";
	fp_Write=fopen(writefile,"w");

	char StrLine[100];
	if((fp_Read = fopen(readfile,"r")) == NULL)
	{
		printf("error");
		return -1;
	}
	while(!feof(fp_Read))
	{
		fgets(StrLine,100,fp_Read);
		printf("%s\n",StrLine);
		StrLine[strlen(StrLine)-1]=NULL;

		char* imageName = StrLine;	
		src = imread(imageName,1); // Read the file

		if(! src.data) // Check for invalid input
		{
			cout << "Could not open or find the image" << std::endl ;
			return -1;
		}
		printf("%s\n",imageName);
		nl=src.rows;
		nc=src.cols;
		namedWindow( "src", CV_WINDOW_AUTOSIZE );

		start=clock();
		gray_process(src);
		
		After_Threshold=Form_Gray;
		//end_time=clock();
		threshold_re(After_Threshold);
		After_Callback=After_Threshold;
		thresh_callback_re(After_Callback);

		find_cross_point(After_Callback);
		end_time=clock();//计时结束
		draw_circle();
		imshow("src", src ); // Show our image inside it.

	
		costtime= (float)(end_time - start)*1000/ CLOCKS_PER_SEC;  //转换时间格式
		printf("\ncost time:%d sec\n",costtime);//显示耗时
		record_data(imageName);

		waitKey(3); // Wait for a keystroke in the window

	}
	fclose(fp_Read);
	fclose(fp_Write);

	getchar();
	return 0;

}





uFORM_ImgParam ConvImg2FormImg(Mat image)
{
	uFORM_ImgParam New_Form_Img;
	New_Form_Img.pSrc=image.data;
	New_Form_Img.nHeight=image.rows;
	New_Form_Img.nWidth=image.cols;
	return New_Form_Img;
}
uFORM_ImgParam Conv2GrayFormImg(Mat Image)
{
	uFORM_ImgParam New_Form_Img;
	int channels=3;
	int i;
	BYTE newdata[560000];//changable
	float parameters[3]={0.114f,0.587f,0.299f};
	
	BYTE *original=Image.data;
	BYTE *newone=newdata;
	for(i=0;i<(nl*nc);i++)
	{
		float number1=original[0]*parameters[0];
		original++;
		float number2=original[0]*parameters[1];
		original++;
		float number3=original[0]*parameters[2];
		original++;
		*newone=(int)(number1+number2+number3);
		newone++;
	}
	New_Form_Img.pSrc=newdata;
	New_Form_Img.nHeight=nl;
	New_Form_Img.nWidth=nc;
	return New_Form_Img;
}

int verify(Mat image,uFORM_ImgParam Form_Image)
{
	int count=0;
	int i,j;
	for( j = 0; j < nl ; j++ )//y
	{ 
		uchar* data=image.ptr<uchar>(j);
		for(  i = 0; i < nc; i++ )//x
		{
			if( abs((int)data[i]-(int)Form_Image.pSrc[j*nc+i])>3)
				count++;
		}
	}
	return count;
}

void gray_process(Mat src)
{
	//cvtColor(src,src_gray,CV_BGR2GRAY);	
	
	Form_Gray=Conv2GrayFormImg(src);
	//int count=verify(src_gray,Form_Gray);
	//cout<<"gray error"<<count<<endl;

	//imshow("Gray image",src_gray);

	
}
void assign_mat(Mat temp_mat,uFORM_ImgParam Form_Image)
{
	int i,j;
	for(j=0;j<nl;j++)
	{
		uchar* data=temp_mat.ptr<uchar>(j);
		for(i=0;i<nc;i++)
			data[i]=(int)Form_Image.pSrc[j*nc+i];
	}
}

void threshold_implement(uFORM_ImgParam Form_Image)
{
	int i;
	BYTE *original=Form_Image.pSrc;
	for(i=0;i<(nl*nc);i++)
	{
		if(*original<threshold_value_re)
			*original=0;
		original++;
	}
}


void threshold_re(uFORM_ImgParam Form_Image)
{

	threshold_implement(Form_Image);

	//Mat temp=Mat::zeros(src_gray.size(),CV_8UC1);
	//assign_mat(temp,Form_Image);
	
	//imshow("threshold_new",temp);
}

BYTE* check(int j,int i,uFORM_ImgParam Form_Image)
{
	return &(Form_Image.pSrc[j*nc+i]);
}
void thresh_callback_implement(uFORM_ImgParam Form_Image)
{

	double up_refer=nl*1.1;
	double down_refer=0;
	double left_refer=nc*1.1;
	double right_refer=0;
	int i,j;
	for(j=0;j<nl;j++)
	{
		for(i=0;i<nc;i++)
		{
			if((int)*(check(j,i,Form_Image))>thresh_detect_image&&j<(nl*0.95)&&i>(nc*0.1))
			{
				if(j<up_refer)
				{
					up_refer=j;
					up_point.x=i;
					up_point.y=j;
				}
				if(j>down_refer)
				{
					down_refer=j;
					down_point.x=i;
					down_point.y=j;
				}
				if(i<left_refer)
				{
					left_refer=i;
					left_point.x=i;
					left_point.y=j;
				}
				if(i>right_refer)
				{
					right_refer=i;
					right_point.x=i;
					right_point.y=j;
				}
			}
		}
	}
}
void assign_point(int x,int y,POINT* temp)
{
	temp[0].x=x;
	temp[0].y=y;
}
void thresh_callback2_implement(uFORM_ImgParam Form_Image)
{

	int up_x_refer=(nc+10),up_y_refer=(nl+10);
	int right_x_refer=0,right_y_refer=(nl+10);
	int down_x_refer=0,down_y_refer=0;
	int left_x_refer=(nc+10),left_y_refer=0;
	int i,j;
	for(j=0;j<nl;j++)
	{
		for(i=0;i<nc;i++)
		{
			if((int)Form_Image.pSrc[j*nc+i]>thresh_detect_image)
			{
				if(j<up_y_refer&&i<up_x_refer)
				{
					up_y_refer=j;
					up_x_refer=i;
				}
				if(j<right_y_refer||i>right_x_refer)
				{
					right_y_refer=j;
					right_x_refer=i;
				}
				if(j>down_y_refer&&i>down_x_refer)
				{
					down_y_refer=j;
					down_x_refer=i;
				}
				if(j>left_y_refer&&i<left_x_refer)
				{
					left_y_refer=j;
					left_x_refer=i;
				}
			}
		}
	}
	assign_point(up_x_refer,up_y_refer,&up_point);
	assign_point(right_x_refer,right_y_refer,&right_point);
	assign_point(down_x_refer,down_y_refer,&down_point);
	assign_point(left_x_refer,left_y_refer,&left_point);

}

double distance_re(POINT A,POINT B)
{
	double distance=0;
	distance=(A.x-B.x)*(A.x-B.x)+(A.y-B.y)*(A.y-B.y);
	distance=sqrt(distance);
	return distance;
}
int thresh_callback_check(void)
{

	if(distance_re(up_point,right_point)<min_45distance||distance_re(up_point,down_point)<min_45distance||distance_re(up_point,left_point)<min_45distance||distance_re(right_point,down_point)<min_45distance||distance_re(right_point,left_point)<min_45distance||distance_re(down_point,left_point)<min_45distance)
		return 1;
	return 0;
}
void draw_circle(void)
{
	circle(src,Point(up_point.x,up_point.y),1,Scalar(0,0,255),2,8,0);
	circle(src,Point(down_point.x,down_point.y),1,Scalar(0,0,255),2,8,0);
	circle(src,Point(left_point.x,left_point.y),1,Scalar(0,0,255),2,8,0);
	circle(src,Point(right_point.x,right_point.y),1,Scalar(0,0,255),2,8,0);

	circle(src,Point(up_point.x,up_point.y),20,Scalar(0,0,255),2,8,0);
	circle(src,Point(down_point.x,down_point.y),20,Scalar(0,0,255),2,8,0);
	circle(src,Point(left_point.x,left_point.y),20,Scalar(0,0,255),2,8,0);
	circle(src,Point(right_point.x,right_point.y),20,Scalar(0,0,255),2,8,0);
}
void show_4point_data(void)
{
	cout<<up_point.x<<" "<<up_point.y<<endl;
	cout<<down_point.x<<" "<<down_point.y<<endl;
	cout<<left_point.x<<" "<<left_point.y<<endl;
	cout<<right_point.x<<" "<<right_point.y<<endl;
}
void thresh_callback_re(uFORM_ImgParam Form_Image)
{
	thresh_callback_implement(Form_Image);
	//show_4point_data();
	int result=thresh_callback_check();
	//int result=1;
	if(result==1)
	{
		//cout<<"find 45 degree image"<<endl;
		thresh_callback2_implement(Form_Image);
		result=thresh_callback_check();
		//show_4point_data();
	}
	if(result==1)
		cout<<"error because of 45 degree"<<endl;
	return;
}
void CrossPoint(const UIMG2DotLine *Line1, const UIMG2DotLine *Line2)  
{  
    POINT pt;  
    // line1's cpmponent 
	double X1 = Line1->ptEn.x - Line1->ptSt.x;//b1
	double Y1 = Line1->ptEn.y - Line1->ptSt.y;//a1

	// line2's cpmponent   
	double X2 = Line2->ptEn.x - Line2->ptSt.x;//b2
	double Y2 = Line2->ptEn.y - Line2->ptSt.y;//a2
    // distance of 1,2  
 	double X21=Line2->ptSt.x - Line1->ptSt.x;
	double Y21=Line2->ptSt.y - Line1->ptSt.y;
	// determinant  
    double D = Y1*X2 - Y2*X1;// a1b2-a2b1  
    if (D == 0) 
	{
		cout<<"D==0"<<endl;
		return;  
	}
    // cross point  
    pt.x = (X1*X2*Y21 + Y1*X2*Line1->ptSt.x - Y2*X1*Line2->ptSt.x)*1.0/ D;  
    // on screen y is down increased !   
    pt.y = -(Y1*Y2*X21 + X1*Y2*Line1->ptSt.y - X2*Y1*Line2->ptSt.y)*1.0 / D;  
    // segments intersect.  
    if ((abs(pt.x - Line1->ptSt.x - X1 / 2) <= abs(X1 / 2)) &&  
        (abs(pt.y - Line1->ptSt.y - Y1 / 2) <= abs(Y1 / 2)) &&  
        (abs(pt.x - Line2->ptSt.x - X2 / 2) <= abs(X2 / 2)) &&  
        (abs(pt.y - Line2->ptSt.y - Y2 / 2) <= abs(Y2 / 2)))  
    {  
		cross_point.x=pt.x;
		cross_point.y=pt.y;
		return;
    }  

    return ;  
}  

void find_cross_point(uFORM_ImgParam Form_Image)
{
	UIMG2DotLine Line1,Line2;
	Line1.ptSt=left_point;
	Line1.ptEn=right_point;
	Line2.ptSt=up_point;
	Line2.ptEn=down_point;

    CrossPoint(&Line1, &Line2);  
	circle(src,Point(cross_point.x,cross_point.y),1,Scalar(0,0,255),2,8,0);
	circle(src,Point(cross_point.x,cross_point.y),10,Scalar(0,0,255),2,8,0);
	//cout<<"cross point x,y:"<<cross_point.x<<" "<<cross_point.y<<endl;
}
void record_data(char* imageName)
{
	fprintf(fp_Write,"%s\n",imageName);
	fprintf(fp_Write,"Point No.1:(%d, %d)\n",up_point.x,up_point.y);
	fprintf(fp_Write,"Point No.2:(%d, %d)\n",right_point.x,right_point.y);
	fprintf(fp_Write,"Point No.3:(%d, %d)\n",down_point.x,down_point.y);
	fprintf(fp_Write,"Point No.4:(%d, %d)\n",left_point.x,left_point.y);
	fprintf(fp_Write,"center point:(%d, %d)\n",cross_point.x,cross_point.y);
	fprintf(fp_Write,"running time is: %d ms\n",costtime);
	return;
}

