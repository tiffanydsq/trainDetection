#include <iostream>
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/core/version.hpp"
#include <opencv2/opencv.hpp>
#include <opencv2/video/background_segm.hpp>

#include <ctime>

using namespace cv;
using namespace std;

int main(int argc, char **argv)
{
    // cout << "Using OpenCV " << CV_MAJOR_VERSION << "." << CV_MINOR_VERSION << "." << CV_SUBMINOR_VERSION << endl;
    
    // check command
    if (argc!=4)
    {
        cout << "Input format: ./GMM input_image_dir RoI_mask_path output_video_path ( > redirect.log) " << endl;
        return -1;
    }
    int scalar = 5;

    vector<String> image_names;
    glob(argv[1]+string("/*.jpg"), image_names);
    Mat RoI_mask=imread(argv[2], 0);

    cout<<"input_image_dir: "<<argv[1]<<endl;
    cout<<"RoI_mask_path: "<<argv[2]<<endl;
    cout<<"output_video_path: "<<argv[3]<<endl;
    Mat frame=imread(image_names[0]);
    VideoWriter video_output(argv[3],
        CV_FOURCC('M','J','P','G'), 
        5, 
        Size(frame.cols/scalar,frame.rows/scalar) 
    );

    int cnt=0;
    Ptr< BackgroundSubtractor> pMOG=createBackgroundSubtractorMOG2(1000, 16, false); //MOG Background subtractor

    // namedWindow("FG mask", cv::WINDOW_NORMAL );
    // resizeWindow("FG mask",1200, 800);  

    // namedWindow("stripe_mask", cv::WINDOW_NORMAL );
    // resizeWindow("stripe_mask",1200, 800); 

    // namedWindow("stripe_detect", cv::WINDOW_NORMAL );
    // resizeWindow("stripe_detect",1200, 800); 

    int num_divid = 10;
    string image_name;
    clock_t begin = clock();
    for(int cnt = 0; cnt < image_names.size(); cnt++)
    {
        clock_t current_begin = clock();
        image_name = image_names[cnt];
        Mat img_input;
        Mat img_mask;

        img_input=imread(image_name);
        if (img_input.empty()) 
        {
            break;  
        }

        // Foreground detection
        pMOG->apply(img_input, img_mask);

        // Apply RoI mask and count foreground pixels
        int width = frame.cols/num_divid;
        float ratio = 0;
        for (int i = 0; i < num_divid; i++)
        {
            Mat stripe_detect, white_mask, white_detect;
            Mat stripe_mask(frame.rows, frame.cols, CV_8UC1, Scalar(0, 0, 0));
            Mat stripe_roi = RoI_mask(Rect(width*i, 0, width, frame.rows));
            stripe_roi.copyTo(stripe_mask(Rect(width*i, 0, width, frame.rows)));
            // imshow("stripe_mask", stripe_mask);
            findNonZero	(stripe_mask, white_mask);
            img_mask.copyTo(stripe_detect, stripe_mask);
            // imshow("stripe_detect", stripe_detect);
            findNonZero	(stripe_detect, white_detect);
            ratio = max(ratio, (float)white_detect.rows/white_mask.rows);
        }
        
        // output to video
        // Mat mask;
        // cvtColor(img_mask, mask, COLOR_GRAY2BGR);
        resize(img_input, img_input, Size(frame.cols/scalar,frame.rows/scalar));
        string text = "P=" + to_string(ratio);
        putText(img_input, text, Point(100/scalar, 200/scalar), FONT_HERSHEY_COMPLEX, 5/scalar, Scalar(0, 255, 0), 10/scalar);
        video_output<<img_input;
        // if (cvWaitKey(30)=='q') 
        //     break;

        if (cnt%10 == 0){
            cout<< "Frame No." << cnt <<endl;
            clock_t current_end = clock();
            double current_elapsed_secs = double(current_end - current_begin) / CLOCKS_PER_SEC;
            cout<< "Time elapsed: "<< current_elapsed_secs << "ms." <<endl;
        }
    }
    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    double average_secs = elapsed_secs / image_names.size();
    cout<< "Average time elapsed per frame is"<< average_secs << "ms." <<endl;
    

    return 0;
}