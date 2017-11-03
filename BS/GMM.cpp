#include <iostream>
#include "opencv2/videoio.hpp"
// #include "opencv2/video.hpp"
#include "opencv2/highgui.hpp"

#include "opencv2/core/version.hpp"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/video/background_segm.hpp>

using namespace cv;
using namespace std;

int main(int argc, char **argv)
{
    std::cout << "Using OpenCV " << CV_MAJOR_VERSION << "." << CV_MINOR_VERSION << "." << CV_SUBMINOR_VERSION << std::endl;

    VideoCapture capture;

    if (argc > 1)
    {
        std::cout << "Openning: " << argv[1] << std::endl;
        capture.open(argv[1]);
    }
    else
        capture.open(0);

    if (!capture.isOpened())
    {
        std::cerr << "Cannot initialize video!" << std::endl;
        return -1;
    }

    string inputname=argv[1];
    VideoWriter video_output(inputname.substr(0,inputname.find_last_of('.'))+string("_fg_MOG2.avi"),
        CV_FOURCC('M','J','P','G'), 
        capture.get(CV_CAP_PROP_FPS), 
        Size(capture.get(CV_CAP_PROP_FRAME_WIDTH),capture.get(CV_CAP_PROP_FRAME_HEIGHT)));


    int key = 0;
    Mat img_input;
    Mat img_mask;
    int cnt=0;
    Ptr< BackgroundSubtractor> pMOG=createBackgroundSubtractorMOG2(); //MOG Background subtractor

    // namedWindow("FG mask", cv::WINDOW_NORMAL );
    // resizeWindow("FG mask",1200, 800);  

    while (key != 'q')
    {
        capture >> img_input;
        if (img_input.empty()) break;

        pMOG->apply(img_input, img_mask, 0.1);

        threshold(img_mask, img_mask, 200, 255, THRESH_BINARY);

        // if(!img_mask.empty())
        //     imshow("FG mask", img_mask);

        Mat white;
        findNonZero	(img_mask,white);
        cout<< white.cols << "  " << white.rows << endl;
        
        // if(cnt==10){
        //     cout<< img_mask ;
        //     cout << endl;
        //     return 0;
        // }
       

        //  process 
        Mat mask;
        cvtColor(img_mask, mask, COLOR_GRAY2BGR);
        video_output<<mask;

        cnt++;
        // if(!(cnt%50))
        cout<<"Frame No."<<cnt<<endl;

        key = cvWaitKey(33);
    }

    capture.release();
    cvDestroyAllWindows();

    return 0;
}

