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
    
    VideoWriter video_flow(inputname.substr(0,inputname.find_last_of('.'))+string("_OF_hist_mag.avi"),
    CV_FOURCC('M','J','P','G'), 
    capture.get(CV_CAP_PROP_FPS), 
    Size(capture.get(CV_CAP_PROP_FRAME_WIDTH)*2,capture.get(CV_CAP_PROP_FRAME_HEIGHT))
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
            vector<Mat1f> OF;
            split(flow, OF);
            Mat magnitude, angle;
            cartToPolar	(OF[0],OF[1],magnitude,angle,true );	//angleInDegrees=true
            // cout<<"magnitude"<<magnitude.cols<<" "<<magnitude.rows<<endl;
            
        /*
            Mat angle_hist;
            // double Min,Max;
            // minMaxLoc(angle, &Min, &Max);
            // cout<<"angle_range:"<<Min<<" "<<Max<<endl;

            int hbins = 36;
            float range[] = { 0, 360 } ; //the upper boundary is exclusive
            const float* histRange = { range };
            int idx_channel=0;
            calcHist( &angle, 1, &idx_channel, Mat(), // do not use mask
                angle_hist, 1, &hbins, &histRange,
                true, // the histogram is uniform
                false );

            // normalize(angle_hist, angle_hist, 0, 1, NORM_MINMAX, -1, Mat() );
            // cout << "angle_hist = "<< angle_hist;
            // cout << endl;

            // namedWindow("pre", WINDOW_NORMAL );
            // resizeWindow("pre", 600,800 );
            // imshow("pre", original);
            

            // Draw  the histograms 
            int hist_w = original.cols; 
            int hist_h = original.rows ;
            int bin_w = cvRound( (double) hist_w/hbins );
            Mat histImage( hist_h, hist_w, CV_8UC3, Scalar( 255,255,255) );

            /// Normalize the result to [ 0, histImage.rows ]
            normalize(angle_hist, angle_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );

            for( int i = 1; i < angle_hist.rows; i++ )
            {
                line( histImage, Point( bin_w*(i-1), hist_h - cvRound(angle_hist.at<float>(i-1)) ) ,
                                 Point( bin_w*(i), hist_h - cvRound(angle_hist.at<float>(i)) ),
                                 Scalar( 255, 0, 0), 2, 8, 0  );
            }

            // namedWindow("angle_hist", WINDOW_NORMAL );
            // resizeWindow("angle_hist",600, 800);
            // imshow("angle_hist", histImage);
            
            Mat merge(original.rows, original.cols+histImage.cols, CV_8UC3);
            // cout<<"merge"<<merge.cols<<" "<<merge.rows<<endl;
            // cout<<"original"<<original.cols<<" "<<original.rows<<endl;
            // cout<<"histImage"<<histImage.cols<<" "<<histImage.rows<<endl;

            Mat left(merge, Rect(0, 0, original.cols , original.rows));
            original.copyTo(left);
            Mat right(merge, Rect(original.cols, 0, histImage.cols, histImage.rows));
            histImage.copyTo(right);

            namedWindow("angle_hist", WINDOW_NORMAL );
            resizeWindow("angle_hist",600*2, 800);
            imshow("angle_hist", merge);

        */

            Mat magnitude_hist;
            double Min,Max;
            minMaxLoc(magnitude, &Min, &Max);
            // cout<<"angle_range:"<<Min<<" "<<Max<<endl;

            int hbins = 20;
            float range[] = { Min, Max } ; //the upper boundary is exclusive
            const float* histRange = { range };
            int idx_channel=0;
            calcHist( &magnitude, 1, &idx_channel, Mat(), // do not use mask
                magnitude_hist, 1, &hbins, &histRange,
                true, // the histogram is uniform
                false );

            // normalize(angle_hist, angle_hist, 0, 1, NORM_MINMAX, -1, Mat() );
            // cout << "angle_hist = "<< angle_hist;
            // cout << endl;

            // namedWindow("pre", WINDOW_NORMAL );
            // resizeWindow("pre", 600,800 );
            // imshow("pre", original);
            

            // Draw  the histograms 
            int hist_w = original.cols; 
            int hist_h = original.rows ;
            int bin_w = cvRound( (double) hist_w/hbins );
            Mat histImage( hist_h, hist_w, CV_8UC3, Scalar( 255,255,255) );

            /// Normalize the result to [ 0, histImage.rows ]
            normalize(magnitude_hist, magnitude_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );

            for( int i = 1; i < magnitude_hist.rows; i++ )
            {
                line( histImage, Point( bin_w*(i-1), hist_h - cvRound(magnitude_hist.at<float>(i-1)) ) ,
                                Point( bin_w*(i), hist_h - cvRound(magnitude_hist.at<float>(i)) ),
                                Scalar( 255, 0, 0), 2, 8, 0  );
            }

            // namedWindow("angle_hist", WINDOW_NORMAL );
            // resizeWindow("angle_hist",600, 800);
            // imshow("angle_hist", histImage);
            
            Mat merge(original.rows, original.cols+histImage.cols, CV_8UC3);
            // cout<<"merge"<<merge.cols<<" "<<merge.rows<<endl;
            // cout<<"original"<<original.cols<<" "<<original.rows<<endl;
            // cout<<"histImage"<<histImage.cols<<" "<<histImage.rows<<endl;

            Mat left(merge, Rect(0, 0, original.cols , original.rows));
            original.copyTo(left);
            Mat right(merge, Rect(original.cols, 0, histImage.cols, histImage.rows));
            histImage.copyTo(right);

            namedWindow("magnitude_hist", WINDOW_NORMAL );
            resizeWindow("magnitude_hist",600*2, 800);
            imshow("magnitude_hist", merge);

            video_flow<<merge;     
        }
        
        // fill previous image
        img.copyTo(prevgray);
        key = waitKey(20);
    }
}
