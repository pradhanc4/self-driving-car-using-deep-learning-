#include <opencv2/opencv.hpp>
#include <raspicam_cv.h>
#include <iostream>
#include <chrono>
#include <ctime>

using namespace std;
using namespace cv;
using namespace raspicam;


//Image processing variables 
Mat frame,Matrix,framePers,frameGray, frameThresh, frameEdge ,frameFinal, frameFinalCvt;
Mat ROILane;
int LeftLanePos,RightLanePos, frameCenter, Result,LaneCenter;


RaspiCam_Cv Camera;

stringstream ss;


vector<int> histrogramLane;
vector<int> histrogramReverse;

Point2f Source[] = {Point2f(35,175),Point2f(320,175),Point2f(45,195), Point2f(334,195)};
Point2f Destination[] = {Point2f(100,0),Point2f(280,0),Point2f(100,240), Point2f(280,240)};

//Point2f Source[] = {Point2f(35,155), Point2f(325,155), Point2f(30,200), Point2f(334,200)};
//Point2f Destination[] = {Point2f(100,0), Point2f(280,0), Point2f(100,240), Point2f(280,240)};

// Machine Learning Variables 
CascadeClassifier Stop_Cascade, Object_Cascade, Traffic_Cascade;
Mat frame_Stop,frame_Stopnew, RoI_Stop, gray_Stop, frame_Object, RoI_Object, gray_Object, frame_Traffic, RoI_Traffic, gray_Traffic,RoI_Stop_sign;
vector<Rect> Stop, Object, Traffic;
int dist_Stop, dist_Object, dist_Traffic;

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
    line(frame,Source[0], Source[1], Scalar(0,300,0), 2);
    line(frame,Source[1], Source[3], Scalar(0,300,0), 2);
    line(frame,Source[3], Source[2], Scalar(0,300,0), 2);
    line(frame,Source[2], Source[0], Scalar(0,300,0), 2); 

    

    Matrix = getPerspectiveTransform(Source, Destination);
    warpPerspective(frame, framePers, Matrix, Size(400,240));




}
void capture()
{
    Camera.grab();
    Camera.retrieve( frame);
    cvtColor(frame, frame_Stop,COLOR_RGB2BGR);
    cvtColor(frame, frame, COLOR_BGR2RGB);
    
}

void Threshold()
{
    
    
    
    cvtColor(framePers, frameGray, COLOR_RGB2GRAY);
    inRange(frameGray,160,255,frameThresh);
    Canny(frameGray, frameEdge, 600,800,3,false);
    add(frameThresh, frameEdge, frameFinal);
    cvtColor(frameFinal, frameFinal, COLOR_GRAY2RGB);

}

void Histrogram()
{
    cout << "frame";
    histrogramLane.resize(400);
    histrogramLane.clear();
    
    //for(int i=0i<frame.size().width;i++)
    for(int i=0; i<400; i++)
    {
            ROILane = frameFinal(Rect(i,140,1,100));
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
    
    
}
    void laneCenter()
    {
                        
                    LaneCenter = (RightLanePos-LeftLanePos)/2 +LeftLanePos;
                    frameCenter = 200;
                    
                    line(frameFinal, Point2f(LaneCenter,0), Point2f(LaneCenter,240), Scalar(0,255,0), 3);
                    line(frameFinal, Point2f(frameCenter,0), Point2f(frameCenter,240), Scalar(255,0,0), 3);
                
                    Result = LaneCenter-frameCenter;
                }    
                
          
    void Stop_detection()
                {
                    if(!Stop_Cascade.load("/home/pi/Desktop/Machine_Learning //Stop_cascade.xml"))
                    {
                  printf("Unable to open stop cascade file");
                    }
                    RoI_Stop = frame_Stop(Rect(0,0,400,240));//200,0,200,140
                    cvtColor(RoI_Stop, gray_Stop, COLOR_RGB2GRAY);
                    equalizeHist(gray_Stop, gray_Stop);
                    Stop_Cascade.detectMultiScale(gray_Stop, Stop);
                    dist_Stop = 0;
                  
                 
                    for(int i=0; i<Stop.size(); i++)
                    {
                          
                  Point P1(Stop[i].x, Stop[i].y);
                  Point P2(Stop[i].x + Stop[i].width, Stop[i].x + Stop[i].height);
                  
                  rectangle(RoI_Stop, P1, P2, Scalar(0, 255, 0), 2);
                  putText(RoI_Stop, "Stop Sign", P1, FONT_HERSHEY_PLAIN, 1,  Scalar(0, 0, 255, 255), 2);
             
                 // std::cout<P2.x-P1.x<std::endl;
                  //dist_Stop = (-2)*(P2.x-P1.x) + 166;//[][][]
                  ss.str(" ");
                  ss.clear();
                  ss<<"Distance = "<<P2.x-P1.x<<" (Pixels)";
                  putText(RoI_Stop, ss.str(), Point2f(1,130), 0, 1, Scalar(0,0,255), 2);	
                  
                    }    
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
   laneCenter();
   Stop_detection();
    
    
    ss.str(" ");
    ss.clear();
    ss<<"Result ="<<Result;
    putText(frame, ss.str(), Point2f(1,50), 0,1, Scalar(0,255,0), 2);
    
    
    
    namedWindow("ORIGINAL",WINDOW_KEEPRATIO);
    moveWindow("ORIGINAL",0,200);
    resizeWindow("ORIGINAL",240,240);
    imshow("ORIGINAL", frame);
    
    namedWindow("Perspective",WINDOW_KEEPRATIO);
    moveWindow("Perspective",320,200);
    resizeWindow("Perspective",240,240);
    imshow("Perspective", framePers);
    
    namedWindow("FINAL",WINDOW_KEEPRATIO);
    moveWindow("FINAL",650,200);
    resizeWindow("FINAL",240,240);
    imshow("FINAL", frameFinal);
    
    
    namedWindow("Stop_sign",WINDOW_KEEPRATIO);
    moveWindow("Stop_sign",1000,200);
    resizeWindow("Stop_sign",240,240);
    imshow("Stop_sign", RoI_Stop);

    
    waitKey(1);
    
    auto end = std::chrono::system_clock::now();
  

    std::chrono::duration<double> elapsed_seconds = end-start;
    
    float t = elapsed_seconds.count();
    int FPS = 1/t;
    cout<<"FPS = "<<FPS<<endl;
    
  
    
    
    
    }

    
    return 0;
     
}
