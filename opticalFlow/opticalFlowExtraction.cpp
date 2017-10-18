#include "opencv2/highgui.hpp"
// #include "opencv2/highgui/h"
#include "opencv2/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/video/tracking.hpp"
#include <vector>
#include <stdio.h>
// #include <Windows.h>
#include <iostream>


using namespace cv;
using namespace std;
int main(int argc, const char** argv)
{
    bool flag_writevideo=true;
    int gridsize=5;
    string inputname=argv[1];

    VideoCapture capture(inputname);

    Mat flow, frame;
    // some faster than mat image container
    UMat  flowUmat, prevgray;

    if (!capture.isOpened())
    {
        cerr << "Cannot initialize video!" << endl;
        return -1;
    }
    
    VideoWriter video_mask(inputname.substr(0,inputname.find_last_of('.'))+string("_OF.avi"),
    CV_FOURCC('M','J','P','G'), 
    capture.get(CV_CAP_PROP_FPS), 
    Size(capture.get(CV_CAP_PROP_FRAME_WIDTH),capture.get(CV_CAP_PROP_FRAME_HEIGHT))
    );
    
   
    int key = 0;
    while (key!=27){ 

        Mat img;
        Mat original;

        capture>>original;
        original.copyTo(img);
        cvtColor(img, img, COLOR_BGR2GRAY);                          

        if (!prevgray.empty()) {
            // calculate optical flow 
                calcOpticalFlowFarneback(prevgray, img, flowUmat, 0.5, 1, 12, 2, 8, 1.2, 0);
            // copy Umat container to standard Mat
            flowUmat.copyTo(flow);

            for (int y = 0; y < original.rows; y += gridsize) {
                for (int x = 0; x < original.cols; x += gridsize){
                    // get the flow from y, x position * 10 for better visibility
                    const Point2f flowatxy = flow.at<Point2f>(y, x) * 10;
                    // draw line at flow direction
                    line(original, Point(x, y), Point(cvRound(x + flowatxy.x), 
                        cvRound(y + flowatxy.y)), Scalar(255,0,0));
                    // draw initial point
                    // circle(original, Point(x, y), 1, Scalar(0, 0, 0), -1);
                }       
            }
            namedWindow("pre", WINDOW_NORMAL );
            resizeWindow("pre",600, 800);
            imshow("pre", original);

            video_mask<<original;     
        }
        
        // fill previous image
        img.copyTo(prevgray);
        key = waitKey(20);
    }
}
