#include <opencv2/opencv.hpp>
#include <raspicam_cv.h>
#include <iostream>
#include <chrono>
#include <ctime>

using namespace std;
using namespace cv;
using namespace raspicam;

//this Mat variable is global 
Mat frame;

Raspicam_cv Camera; //this is camera object which is declare as global variable to access all fuctions 


//point2f is opencv point structure and this declare as dynamic arryas 
point2f Source[] = (point2f(50,200),point2f(280,200), point2f(0,240), point2f(0,240));//this two floating point our cordinate which x and y axix;
//this is for region of interest 
//syntax:name of array =(point2f(x,y) direciton, point2f(x,y) direction ,point2f(x,y)buttom lef corner,point2f(x,y) buttom right corner ));     

point2f Destination[] = (point2f(50,200),point2f(280,200), point2f(0,240), point2f(0,240));
//this is for perspective wrap 
//all argument same 

 void Setup ( int argc,char **argv, RaspiCam_Cv &Camera )
  {
    Camera.set ( CAP_PROP_FRAME_WIDTH,  ( "-w",argc,argv,400 ) );
    Camera.set ( CAP_PROP_FRAME_HEIGHT,  ( "-h",argc,argv,240 ) );
    Camera.set ( CAP_PROP_BRIGHTNESS, ( "-br",argc,argv,50 ) );
    Camera.set ( CAP_PROP_CONTRAST ,( "-co",argc,argv,50 ) );
    Camera.set ( CAP_PROP_SATURATION,  ( "-sa",argc,argv,50 ) );
    Camera.set ( CAP_PROP_GAIN,  ( "-g",argc,argv ,50 ) );
    Camera.set ( CAP_PROP_FPS,  ( "-fps",argc,argv,100));

}
// 1 
// start create a fuction for capture videos and frames 
void Capture() // this capture fuction called in while loop then it will execute 
{

Camera.grab();
    Camera.retrieve( frame);
    cvtColor(frame, frame ,COLOR_BGR2RGB)//this cvtColor function is 
      //syntax:input image ,outpute image,preset) 
      //also check this Opencv link to more details 
      //Link:https://docs.opencv.org/4.x/d8/d01/group__imgproc__color__conversions.html#ga397ae87e1288a81d2363b61574eb8cab
      // In this link all imformation available 
}

// point2f all four point joined into one frame 
//this is perspective fuction declaration 
void perspective()// this fuction call in while loop
{
    //this is for Rigion of interest 
    //join 4 point for we use line feature for opencv 
    //defining arguments 
    //1)image= frame
    //2)sourse=1st point array defined as [0];
    //3)sourse=2nd point array defined as [1];
    //4)scalar=defining for color line with scalar and opencv image processing is BGR COlor space 
    // scalar=(0,0,0)   so 1st 0 is for blue channel,2nd is for green channel,3rd is for red channel 
    //Note:i am using red color so thats why i am changing of 3rd 0
    //5)width=defining width of over line with 2 pixel
line=(frame,sourse[0],sourse[2],scalar(0,0,255),2);// 1st line is for point zero to two
line=(frame,sourse[1],sourse[3],scalar(0,0,255),2);// 1st line is for point one to three
line=(frame,sourse[3],sourse[2],scalar(0,0,255),2);// 1st line is for point three to two
line=(frame,sourse[2],sourse[0],scalar(0,0,255),2);// 1st line is for point two to zero

//all line for to make region of interest 




//this is cammand for perspective task of region of interest 
//so this is cammand for perspective transformation to region of intrest 
matrix = getPerspectiveTransform(sourse,destination)
//this cammand has two arguments 
//1)sourse cordinate 
//2)destination cordinate 
//using matrix variable to create new frame perspective image and matrix variable declared in global variable

//for create new perspective image use diffent cammand which is warpPerspective
//we use warpPerspective
//this is cammand have 4 arguments 
//1)input image----our input image is frame
//2) output image---our output image is frameperspective
//3)transformation----our trasformation is matrix
//4) size -----with the size function set frame size ..this is our frame resolution 
warpPerspective = (frame,frameperspective,matrix,size(350,240));
//this cammand find also on opencv documentation





}

int main(int argc,char **argv)
{
	
        RaspiCam_Cv Camera;
	Setup(argc, argv, Camera);
	cout<<"Connecting to camera"<<endl;
	if (!Camera.open())
	{
		
	cout<<"Failed to Connect"<<endl;
     }
     
     cout<<"Camera Id = "<<Camera.getId()<<endl;
     
     
     
    
    while(1)
    {
    auto start = std::chrono::system_clock::now();
    capture(); //fuction call 


    //this is aur orignal RGB image
    namedWindow("ORIGINAL",WINDOW_KEEPRATIO);
    moveWindow("ORIGINAL",0,100);
    resizeWindow("ORIGINAL",480,480);
    imshow("ORIGINAL", frame);
    
    //this is our perspective frame
    namedWindow("Perspective",WINDOW_KEEPRATIO);
    moveWindow("Perspective",640,100);
    resizeWindow("Perspective",640,480);
    imshow("Perspective", framePers);
    
    namedWindow("FINAL",WINDOW_KEEPRATIO);
    moveWindow("FINAL",300,100);
    resizeWindow("FINAL",640,480);
    imshow("FINAL", frameFinal);



    auto end = std::chrono::system_clock::now();
  

    std::chrono::duration<double> elapsed_seconds = end-start;
    
    float t = elapsed_seconds.count();
    int FPS = 1/t;
    cout<<"FPS = "<<FPS<<endl;
    imshow("orignal", frame);
  
    waitKey(1);
    
    
    }

    
    return 0;
     
}
