//final got the 5 keypoints stored in global variable:finalpoints.
#include <io.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <cv.h>
#include<time.h>
#include<ctime>
#include <highgui.h>
using namespace cv;
using namespace std;
/// Global variables
double rate=0.028;
int threshold_for_count=200;
int threshold_value2=230;
int threshold_value = 150;//for threshold
int thresh_contour = 60;
int thresh_for_rotate_contour=50;
int thresh_detect_image =160;
int max_thresh=255;
int min_45distance=15; //need to be verified
int threshold_type = 3;
int const max_BINARY_value = 255;
double leftx,rightx,uppery,lowy;
Point uppery_point,lowy_point,leftx_point,rightx_point;
Point A,B,C,D;//for 45 degree situation
Point finalpoints[5];
Point closeA,closeB,closeC,closeD;//for 45 degree situation
Point cross;
RNG rng(12345);
clock_t start;
clock_t end_time;
Mat src, src_gray,after_threshold,drawing,drawing2;

struct PT  
{  
    int x;  
    int y;  
};  
struct LINE  
{  
    PT pStart;  
    PT pEnd;  
};  

/// Function headers
void Threshold_Demo( int, void* );//include load and gray operation
float powf(float x,float y);
double abs(double x);
double fabs(double x);
double sqrt(double x);
Point CrossPoint(const LINE *line1, const LINE *line2);
ofstream record;
void thresh_callback(int, void* );
int draw_circle(void);
int draw_circle2(void);
int judgement45(Point a,Point b,Point c,Point d);
double distances(Point a,Point b);
void thresh_callback2(int, void* );
void result_not_45(void);
void result_45(void);
void count_noise_point(int, void*);
void final_show(int);
void getFiles( string path, vector<string>& files ) ;
int main( int argc, char** argv )
{

	char * filePath = "D:\\internship\\test";  
	vector<string> files;  

	////获取该路径下的所有文件  
	getFiles(filePath, files );  
	ofstream record;
	record.open("D:\\internship\\test\\test2.txt");

	char str[70];  
	int size = files.size();  
	for (int i = 0;i < size;i++)  
	{  
		src = imread( files[i].c_str());
		if (!src.data)
			continue;
		cout<<" "<<endl;
		cout<<files[i].c_str()<<endl; 
		record<<files[i].c_str()<<endl; 
		/// Load an image

		start=clock();
		cvtColor( src, src_gray, CV_RGB2GRAY );

		count_noise_point(0,0);

		Threshold_Demo( 0, 0 );

		
		thresh_callback(0,0);
		
		int result45=draw_circle();
		if(result45==1)
			result_45();	
		end_time=clock();
		
		final_show(result45);
		waitKey(0);
		record<<"the four boundary points's location is:"<<endl;
		record<<"boundary point No.1:("<<finalpoints[0].x<<" , "<<finalpoints[0].y<<")"<<endl;
		record<<"boundary point No.2:("<<finalpoints[1].x<<" , "<<finalpoints[1].y<<")"<<endl;
		record<<"boundary point No.3:("<<finalpoints[2].x<<" , "<<finalpoints[2].y<<")"<<endl;
		record<<"boundary point No.4:("<<finalpoints[3].x<<" , "<<finalpoints[3].y<<")"<<endl;
		record<<"center point is:("<<cross.x<<" , "<<cross.y<<")"<<endl;
		cout<< "Running time is: "<<static_cast<double>(end_time-start)/CLOCKS_PER_SEC*1000<<"ms"<<endl;//输出运行时间
		record<<"Running time is: "<<static_cast<double>(end_time-start)/CLOCKS_PER_SEC*1000<<"ms"<<endl;//输出运行时间
		record<<" "<<endl;
		threshold_value=150;
	}  
	record.close();
	waitKey(0);
	return 0;
	
}
void getFiles( string path, vector<string>& files )  
{  
    //文件句柄  
    long   hFile   =   0;  
    //文件信息  
    struct _finddata_t fileinfo;  
    string p;  
    if((hFile = _findfirst(p.assign(path).append("\\*").c_str(),&fileinfo)) !=  -1)  
    {  
        do  
        {  
            //如果是目录,迭代之  
            //如果不是,加入列表  
            if((fileinfo.attrib &  _A_SUBDIR))  
            {  
                if(strcmp(fileinfo.name,".") != 0  &&  strcmp(fileinfo.name,"..") != 0)  
                    getFiles( p.assign(path).append("\\").append(fileinfo.name), files );  
            }  
            else  
                files.push_back(p.assign(fileinfo.name) );  
        }while(_findnext(hFile, &fileinfo)  == 0);  
        _findclose(hFile);  
    }  
}  

void Threshold_Demo( int, void* )
{
	threshold( src_gray, after_threshold, threshold_value, max_BINARY_value,threshold_type );
}


Point CrossPoint(const LINE *line1, const LINE *line2)  
{  
    Point pt;  
    // line1's cpmponent  
    double X1 = line1->pEnd.x - line1->pStart.x;//b1  
    double Y1 = line1->pEnd.y - line1->pStart.y;//a1  
    // line2's cpmponent  
    double X2 = line2->pEnd.x - line2->pStart.x;//b2  
    double Y2 = line2->pEnd.y - line2->pStart.y;//a2  
    // distance of 1,2  
    double X21 = line2->pStart.x - line1->pStart.x;  
    double Y21 = line2->pStart.y - line1->pStart.y;  
    // determinant  
    double D = Y1*X2 - Y2*X1;// a1b2-a2b1  
    //   
    if (D == 0) return 0;  
    // cross point  
    pt.x = (X1*X2*Y21 + Y1*X2*line1->pStart.x - Y2*X1*line2->pStart.x) / D;  
    // on screen y is down increased !   
    pt.y = -(Y1*Y2*X21 + X1*Y2*line1->pStart.y - X2*Y1*line2->pStart.y) / D;  
    // segments intersect.  
    if ((abs(pt.x - line1->pStart.x - X1 / 2) <= abs(X1 / 2)) &&  
        (abs(pt.y - line1->pStart.y - Y1 / 2) <= abs(Y1 / 2)) &&  
        (abs(pt.x - line2->pStart.x - X2 / 2) <= abs(X2 / 2)) &&  
        (abs(pt.y - line2->pStart.y - Y2 / 2) <= abs(Y2 / 2)))  
    {  
        return pt;  
    }  
    return 0;  
}  

void thresh_callback(int, void* )
{
	Mat threshold_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	/// Detect edges using Threshold
	
	threshold( after_threshold, threshold_output, thresh_contour, 255, THRESH_BINARY );
	
	/// Find contours
	findContours( threshold_output, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_L1, Point(0, 0) );
	
	/// Approximate contours to polygons + get bounding rects and circles
	vector<vector<Point> > contours_poly( contours.size() );
	vector<Rect> boundRect( contours.size() );
	vector<Point2f>center( contours.size() );
	//vector<float>radius( contours.size() );
	for( int i = 0; i < contours.size(); i++ )
	{ 
		approxPolyDP( Mat(contours[i]), contours_poly[i], 0, true );
		boundRect[i] = boundingRect( Mat(contours_poly[i]) );
		//minEnclosingCircle( (Mat)contours_poly[i], center[i], radius[i] );
	}
	
	int max_i=0;
	double max_area=0;

	for(int i = 0; i< contours.size(); i++ )
	{
		if(boundRect[i].area()>max_area)
		{
			max_i=i;
			max_area=boundRect[i].area();
		}	
	}

	leftx=boundRect[max_i].tl().x;
	rightx=boundRect[max_i].br().x;
	uppery=boundRect[max_i].tl().y;
	lowy=boundRect[max_i].br().y;
}

int draw_circle(void)
{
	double distance_leftx=10000;
	double distance_rightx=10000;
	double distance_uppery=10000;
	double distance_lowy=10000;
	
	int nl=after_threshold.rows;
	int nc=after_threshold.cols;

	//start=clock();
	for( int j = 0; j < nl ; j++ )//y
	{ 
		uchar* data=after_threshold.ptr<uchar>(j);
		for( int i = 0; i < nc; i++ )//x
		{
			if( (int)data[i]>thresh_detect_image&&j<(nl*0.95)&&i>(nc*0.1))
			{
				if(abs(j-uppery)<distance_uppery)
				{
					distance_uppery=abs(j-uppery);
					uppery_point=Point(i,j);
					continue;
				}
				if(abs(j-lowy)<distance_lowy)
				{
					distance_lowy=abs(j-lowy);
					lowy_point=Point(i,j);
					continue;
				}
				if(abs(i-leftx)<distance_leftx)
				{
					distance_leftx=abs(i-leftx);
					leftx_point=Point(i,j);
					continue;
				}
				if(abs(i-rightx)<distance_rightx)
				{
					distance_rightx=abs(i-rightx);
					rightx_point=Point(i,j);
					continue;
				}
			}
		}
	}
	//end_time=clock();
    LINE line1, line2;  
	line1.pStart.x = leftx_point.x;  
	line1.pStart.y = leftx_point.y;  
    line1.pEnd.x = rightx_point.x;  
	line1.pEnd.y = rightx_point.y;  
	line2.pStart.x = uppery_point.x;  
	line2.pStart.y = uppery_point.y;  
	line2.pEnd.x = lowy_point.x;  
	line2.pEnd.y = lowy_point.y; 
    cross = CrossPoint(&line1, &line2);  
	int result45 =judgement45(uppery_point,lowy_point,leftx_point,rightx_point);
	return result45;
}

int judgement45(Point a,Point b,Point c,Point d)
{
	int result;
	if((distances(a,b)<min_45distance)||(distances(a,c)<min_45distance)||(distances(a,d)<min_45distance)||(distances(b,c)<min_45distance)||(distances(b,d)<min_45distance)||(distances(c,d)<min_45distance))
		return 1;//find 45  degree
	return 0; //fail to find 45 degeree
}

double distances(Point a,Point b)
{
	return (double)sqrt(powf((a.x-b.x),2)+powf((a.y-b.y),2));
}

void thresh_callback2(int, void* )
{
	Mat threshold_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	/// Detect edges using Threshold
	threshold( after_threshold, threshold_output, thresh_for_rotate_contour, 255, THRESH_BINARY );
	/// Find contours
	findContours( threshold_output, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_L1, Point(0, 0) );

	/// Find the rotated rectangles and ellipses for each contour
	vector<RotatedRect> minRect( contours.size() );
	vector<RotatedRect> minEllipse( contours.size() );

	for( size_t i = 0; i < contours.size(); i++ )
		minRect[i] = minAreaRect( Mat(contours[i]) );

	/// Draw contours + rotated rects + ellipses
	//drawing2 = Mat::zeros( threshold_output.size(), CV_8UC3 );
	int max_i=0;
	double max_crossline=0;
	for( size_t i = 0; i< contours.size(); i++ )
	{
		Point2f rect_points[4]; 
		minRect[i].points( rect_points );

		if(distances(rect_points[0],rect_points[2])>max_crossline)
		{
			max_i=i;
			max_crossline=distances(rect_points[0],rect_points[2]);
		}

	}
	Point2f rect_points[4];
	minRect[max_i].points( rect_points );

	A=rect_points[0];
	B=rect_points[1];
	C=rect_points[2];
	D=rect_points[3];
}

int draw_circle2(void)
{
	double distance_A=10000;
	double distance_B=10000;
	double distance_C=10000;
	double distance_D=10000;
	
	int nl=after_threshold.rows;
	int nc=after_threshold.cols*after_threshold.channels();
	for( int j = 0; j < nl ; j++ )//y
	{ 
		uchar* data=after_threshold.ptr<uchar>(j);
		for( int i = 0; i < nc; i++ )//x
		{
			if( (int)data[i]>thresh_detect_image&&j<(nl*0.95)&&i>(after_threshold.cols*0.1))
			{
				if(distances(Point(i,j),A)<distance_A)
				{
					closeA=Point(i,j);
					distance_A=distances(Point(i,j),A);
					continue;
				}
				if(distances(Point(i,j),B)<distance_B)
				{
					closeB=Point(i,j);
					distance_B=distances(Point(i,j),B);
					continue;
				}
				if(distances(Point(i,j),C)<distance_C)
				{
					closeC=Point(i,j);
					distance_C=distances(Point(i,j),C);
					continue;
				}
				if(distances(Point(i,j),D)<distance_D)
				{
					closeD=Point(i,j);
					distance_D=distances(Point(i,j),D);
					continue;
				}
			}
		}
	}

    LINE line1, line2;  
	line1.pStart.x = closeA.x;  
	line1.pStart.y = closeA.y;  
    line1.pEnd.x = closeC.x;  
	line1.pEnd.y = closeC.y;  
	line2.pStart.x = closeB.x;  
	line2.pStart.y = closeB.y;  
	line2.pEnd.x = closeD.x;  
	line2.pEnd.y = closeD.y;   
    cross = CrossPoint(&line1, &line2);  

	int result45 =judgement45(closeA,closeB,closeC,closeD);
	return result45;
}


void result_45(void)
{
	thresh_callback2(0,0);
	int result45=draw_circle2();
}

void final_show(int result45)
{
	if(result45==1)
	{
		//draw big circles to draw attention
		circle(src, closeA, 20, Scalar(0,0,255), 2, 8, 0 );
		circle(src, closeB, 20, Scalar(0,0,255), 2, 8, 0 );
		circle(src, closeC, 20, Scalar(0,0,255), 2, 8, 0 );
		circle(src, closeD, 20, Scalar(0,0,255), 2, 8, 0 );
		circle(src, cross, 20, Scalar(0,0,255), 2, 8, 0 );
		//draw small dots.
		circle(src, closeA, 1, Scalar(0,0,255), 2, 8, 0 );
		circle(src, closeB, 1, Scalar(0,0,255), 2, 8, 0 );
		circle(src, closeC, 1, Scalar(0,0,255), 2, 8, 0 );
		circle(src, closeD, 1, Scalar(0,0,255), 2, 8, 0 );
		circle(src, cross, 1, Scalar(0,0,255), 2, 8, 0 );
		finalpoints[0]=closeA;
		finalpoints[1]=closeB;
		finalpoints[2]=closeC;
		finalpoints[3]=closeD;
		finalpoints[4]=cross;
	}
	else
	{
		//draw big dots
		circle(src, uppery_point, 20, Scalar(0,0,255), 2, 8, 0 );
		circle(src, lowy_point, 20, Scalar(0,0,255), 2, 8, 0 );
		circle(src, leftx_point, 20, Scalar(0,0,255), 2, 8, 0 );
		circle(src,  rightx_point, 20, Scalar(0,0,255), 2, 8, 0 );
		circle(src, cross, 20, Scalar(0,0,255), 2, 8, 0 );
		//draw small dots.
		circle(src, uppery_point, 1, Scalar(0,0,255), 2, 8, 0 );
		circle(src, lowy_point, 1, Scalar(0,0,255), 2, 8, 0 );

		circle(src, leftx_point, 1, Scalar(0,0,255), 2, 8, 0 );
		circle(src, rightx_point, 1, Scalar(0,0,255), 2, 8, 0 );
		circle(src, cross, 1, Scalar(0,0,255), 2, 8, 0 );
		finalpoints[0]=uppery_point;
		finalpoints[1]=lowy_point;
		finalpoints[2]=leftx_point;
		finalpoints[3]=rightx_point;
		finalpoints[4]=cross;
	}
	namedWindow( "final", WINDOW_AUTOSIZE );
	imshow( "final", src );
	//imwrite("final.jpg",src);
	return;
}

void count_noise_point(int, void*)
{
	int count_number=0;
	int nl=src_gray.rows;
	int nc=src_gray.cols*src_gray.channels();
	for( int j = 0; j < nl ; j++ )//y
	{ 
		uchar* data=src_gray.ptr<uchar>(j);
		for( int i = 0; i < nc; i++ )//x
		{
			if((int)data[i]>threshold_for_count)//
			{
				count_number++;
				//out<<(int)data[i]<<" ";
			}
				
		}
	}
	if((double)count_number>(double)(nl*nc*rate))
	{
		//cout<<"count_number:"<<count_number<<endl;
		//cout<<"nl*nc*rate:"<<(double)(nl*nc*rate)<<endl;
		//cout<<"too noisy"<<endl;
		threshold_value=threshold_value2;
	}
		
	return;//not noisy
}
