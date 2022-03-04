#include <opencv2/opencv.hpp>
#include <raspicam_cv.h>
#include <iostream>
#include <chrono>
#include <ctime>

using namespace std;
using namespace cv;
using namespace raspicam;

Mat frame,Matrix,framePers,frameGray, frameThresh, frameEdge ,frameFinal,laneCenter;
Mat ROILane;
int LeftLanePos,RightLanePos;


RaspiCam_Cv Camera;

vector<int> histrogramLane;

Point2f Source[] = {Point2f(35,135), Point2f(295,130), Point2f(0,180), Point2f(340,180)};
Point2f Destination[] = {Point2f(80,0), Point2f(280,0), Point2f(80,240), Point2f(280,240)};


 void Setup ( int argc,char **argv, RaspiCam_Cv &Camera )
  {
    Camera.set ( CAP_PROP_FRAME_WIDTH,  ( "-w",argc,argv,400 ) );
    Camera.set ( CAP_PROP_FRAME_HEIGHT,  ( "-h",argc,argv,240 ) );
    Camera.set ( CAP_PROP_BRIGHTNESS, ( "-br",argc,argv,50 ) );
    Camera.set ( CAP_PROP_CONTRAST ,( "-co",argc,argv,50 ) );
    Camera.set ( CAP_PROP_SATURATION,  ( "-sa",argc,argv,50 ) );
    Camera.set ( CAP_PROP_GAIN,  ( "-g",argc,argv ,50 ) );
    Camera.set ( CAP_PROP_FPS,  ( "-fps",argc,argv,0));

}
void Perspective()
{
    line(frame,Source[0], Source[1], Scalar(0,0,255), 2);
    line(frame,Source[1], Source[3], Scalar(0,0,255), 2);
    line(frame,Source[3], Source[2], Scalar(0,0,255), 2);
    line(frame,Source[2], Source[0], Scalar(0,0,255), 2); 

    

    Matrix = getPerspectiveTransform(Source, Destination);
    warpPerspective(frame, framePers, Matrix, Size(350,240));




}
void capture()
{
    Camera.grab();
    Camera.retrieve( frame);
    cvtColor(frame, frame, COLOR_BGR2RGB);
}

void Threshold()
{
    cvtColor(framePers, frameGray, COLOR_RGB2GRAY);
    inRange(frameGray,110,250,frameThresh);
    Canny(frameGray, frameEdge, 100,350,3,false);
    add(frameThresh, frameEdge, frameFinal);
    cvtColor(frameFinal, frameFinal, COLOR_GRAY2RGB);

}

void Histrogram()
{
    histrogramLane.resize(400);
    histrogramLane.clear();
    
    //for(int i=0i<frame.size().width;i++)
    for(int i=0; i<400; i++)
    {
            ROILane = frameFinal(Rect(i,140,1,240));
            divide(255,ROILane, ROILane);
            histrogramLane.push_back((int)(sum(ROILane)[0]));
    }
}

void LaneFinder()
{
                    vector<int>:: iterator LeftPtr;
                    LeftPtr = max_element(histrogramLane.begin(), histrogramLane.begin()+190);
                    LeftLanePos = distance(histrogramLane.begin(), LeftPtr); 
                    
                    vector<int>:: iterator RightPtr;
                    
                    histrogramReverse.resize(400);
                    
                    histrogramReverse.assign(histrogramLane.begin(), histrogramLane.end());
                    reverse(histrogramReverse.begin(), histrogramReverse.end()); 
                    
                    
                    RightPtr = max_element(histrogramReverse.begin(), histrogramReverse.begin()+190);
                    RightLanePos = 400 - distance(histrogramReverse.begin(), RightPtr);
                    
                    line(frameFinal, Point2f(LeftLanePos, 0), Point2f(LeftLanePos, 240), Scalar(0, 255,0), 2);
                    line(frameFinal, Point2f(RightLanePos, 0), Point2f(RightLanePos, 240), Scalar(0,255,0), 2); 
    
    // lane center

     laneCenter = (RightLanePos-LeftLanePos)/2 +LeftLanePos;
                    frameCenter = 188;
                    
                    line(frameFinal, Point2f(laneCenter,0), Point2f(laneCenter,240), Scalar(0,255,0), 3);
                    line(frameFinal, Point2f(frameCenter,0), Point2f(frameCenter,240), Scalar(255,0,0), 3);
                
                    Result = laneCenter-frameCenter;
    
    
    
}
    
    
    


int main(int argc,char **argv)
{
	
        
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
    
    capture();
    Perspective();
    Threshold();
    Histrogram();
    LaneFinder();
    
    
    namedWindow("ORIGINAL",WINDOW_KEEPRATIO);
    moveWindow("ORIGINAL",0,100);
    resizeWindow("ORIGINAL",480,480);
    imshow("ORIGINAL", frame);
    
    namedWindow("Perspective",WINDOW_KEEPRATIO);
    moveWindow("Perspective",640,100);
    resizeWindow("Perspective",640,480);
    imshow("Perspective", framePers);
    
    namedWindow("FINAL",WINDOW_KEEPRATIO);
    moveWindow("FINAL",300,100);
    resizeWindow("FINAL",640,480);
    imshow("FINAL", frameFinal);
    
    waitKey(1);
    
    auto end = std::chrono::system_clock::now();
  

    std::chrono::duration<double> elapsed_seconds = end-start;
    
    float t = elapsed_seconds.count();
    int FPS = 1/t;
    cout<<"FPS = "<<FPS<<endl;
    
  
    
    
    
    }

    
    return 0;
     
}
