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
    // cout << "Using OpenCV " << CV_MAJOR_VERSION << "." << CV_MINOR_VERSION << "." << CV_SUBMINOR_VERSION << endl;
    
    // check command
    if (argc!=4)
    {
        cout << "Input format: ./GMM input_image_dir RoImask_path output_video_path > redirect.log" << endl;
        return -1;
    }

    vector<String> image_names;
    glob(argv[1]+string("/*.jpg"), image_names);
    Mat RoI_mask=imread(argv[2], 0);

    cout<<"input_image_dir: "<<argv[1]<<endl;
    cout<<"RoImask_path: "<<argv[2]<<endl;
    cout<<"output_video_path: "<<argv[3]<<endl;
    Mat frame=imread(image_names[0]);
    VideoWriter video_output(argv[3],
        CV_FOURCC('M','J','P','G'), 
        5, 
        Size(frame.cols,frame.rows) 
    );


    int cnt=0;
    Ptr< BackgroundSubtractor> pMOG=createBackgroundSubtractorMOG2(); //MOG Background subtractor

    // namedWindow("FG mask", cv::WINDOW_NORMAL );
    // resizeWindow("FG mask",1200, 800);  

    for(string image_name:image_names)
    {
        Mat img_input;
        Mat img_mask;

        img_input=imread(image_name);
        if (img_input.empty()) 
        {
            cout<< "Error loading current image"<<endl;
            break;  
        }
        cnt++;

        // Foreground detection
        pMOG->apply(img_input, img_mask);
        threshold(img_mask, img_mask, 200, 255, THRESH_BINARY);
        // if(!img_mask.empty())
        //     imshow("FG mask", img_mask);

        // Apply RoI mask and count foreground pixels
        Mat img_masked;
        img_mask.copyTo(img_masked, RoI_mask);

        // bitwise_and(img_mask, RoI_mask, img_mask);
        Mat white;
        findNonZero(img_masked,white);
        cout<<"Frame No."<<cnt<< ", num_pixel=" << white.rows << endl;
        
        // output to video
        Mat mask;
        cvtColor(img_masked, mask, COLOR_GRAY2BGR);
        video_output<<mask;

        if (cvWaitKey(30)=='q') 
            break;
    }
    cvDestroyAllWindows();

    return 0;
}

