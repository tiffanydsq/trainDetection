#include <iostream>
#include <fstream>
// #include <algorithm>
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/core/version.hpp"
#include <opencv2/opencv.hpp>
#include <opencv2/video/background_segm.hpp>

#include <math.h>
#include <chrono>

using namespace cv;
using namespace std;

int main(int argc, char **argv)
{
    // cout << "Using OpenCV " << CV_MAJOR_VERSION << "." << CV_MINOR_VERSION << "." << CV_SUBMINOR_VERSION << endl;
    
    // check command
    if (argc!=4)
    {
        cout << "Input format: ./GMM_LowFPS input_image_folder RoI_mask_path output_folder ( > redirect.log) " << endl;
        return -1;
    }
    int scalar = 5;

    vector<String> image_names;
    glob(argv[1]+string("/*.jpg"), image_names);
    Mat RoI_mask_full=imread(argv[2], 0);

    cout<<"input_image_folder: "<<argv[1]<<endl;
    cout<<"RoI_mask_path: "<<argv[2]<<endl;
    string output_video_path = argv[3] + string("/detect.avi");
    cout<<"output_video_path: "<<output_video_path<<endl;
    Mat frame=imread(image_names[0]);
    VideoWriter video_output(output_video_path,
        CV_FOURCC('M','J','P','G'), 
        5, 
        Size(frame.cols/scalar,frame.rows/scalar) 
    );

    int cnt=0;
    Ptr< BackgroundSubtractor> pMOG=createBackgroundSubtractorMOG2(800, 20, false); //MOG Background subtractor

    // namedWindow("FG mask", WINDOW_NORMAL );
    // resizeWindow("FG mask",1200, 800);  

    int num_divid = 10;
    float width_init = 0.15;
    float width_scalar = 0.9;
    float threshold = 0.3; // ratio of fg pixels
    int ROI_mask_offset_X = 0;
    int ROI_mask_offset_Y = 0; //688;
    int ROI_mask_offset_width = 4112;
    int ROI_mask_offset_height = 3008; //1912;
    ofstream frame_log;
    ofstream detect_log;
    frame_log.open (argv[3]+string("/frame.log"));
    detect_log.open (argv[3]+string("/detect.log"));


    Mat RoI_mask;
    RoI_mask = RoI_mask_full(Rect( ROI_mask_offset_X, ROI_mask_offset_Y, ROI_mask_offset_width, ROI_mask_offset_height));
    int widths[num_divid];
    int width_start = 0; //= frame.cols/num_divid;
    int num_pixel_mask[num_divid];
    vector<Mat> stripe_masks;
    for (int i = 0; i < num_divid; i++)
    {
        if (i >0)
            width_start+= widths[i-1];
        widths[i] = (int)round(frame.cols * width_init * pow(width_scalar, i));
        if (widths[i] > frame.cols-width_start)
            break;
        Mat white_mask;
        Mat stripe_mask(frame.rows, widths[i], CV_8UC1, Scalar(0, 0, 0));

        stripe_mask = RoI_mask(Rect( width_start, 0, widths[i], frame.rows));
        stripe_masks.push_back(stripe_mask);
        findNonZero	(stripe_mask, white_mask);
        num_pixel_mask[i] = white_mask.rows;
    }
    
    string image_name;
    auto begin = chrono::system_clock::now();
    for(int cnt = 0; cnt < image_names.size(); cnt++)
    {
        // auto current_begin = chrono::system_clock::now();
        image_name = image_names[cnt];
        Mat img_input;
        Mat img_mask;

        img_input=imread(image_name);
        if (img_input.empty()) 
        {
            break;  
        }

        // Foreground detection
        // auto current_begin = chrono::system_clock::now();
        pMOG->apply(img_input, img_mask);
        // auto current_end = chrono::system_clock::now();

        // Apply RoI mask and count foreground pixels
        float ratio = 0;
        width_start = 0;
        for (int i = 0; i < num_divid; i++)
        {
            if (i > 0)
                width_start+= widths[i-1];
            if (widths[i] > frame.cols-width_start)
                break;
            Mat white_detect, stripe_detect_masked;
            Mat stripe_detect(frame.rows, widths[i], CV_8UC1, Scalar(0, 0, 0));
            stripe_detect = img_mask(Rect(width_start, 0, widths[i], frame.rows));
            stripe_detect.copyTo(stripe_detect_masked, stripe_masks[i]);
            findNonZero	(stripe_detect_masked, white_detect);
            // cout <<white_detect.rows<<endl;
            ratio = max(ratio, (float)white_detect.rows/num_pixel_mask[i]);
        }
        // auto current_end = chrono::system_clock::now();

        // Save to video
        resize(img_input, img_input, Size(frame.cols/scalar,frame.rows/scalar));
        string text = "Frame No." + to_string(cnt) + ", P=" + to_string(ratio);
        if(ratio > threshold)
            detect_log<< "Train detected at frame No." << cnt << "\t" << text <<endl;
        frame_log <<  "Frame No." << cnt << "\t" << text <<endl;
        putText(img_input, text, Point(100/scalar, 200/scalar), FONT_HERSHEY_COMPLEX, 5/scalar, Scalar(0, 255, 0), 10/scalar);
        video_output<<img_input;
       


        //// Display foreground mask
        // Mat mask;
        // cvtColor(img_mask, mask, COLOR_GRAY2BGR);
        // imshow("FG mask", mask);
        // if (cvWaitKey(30)=='q') 
        //     break;

        if (cnt%100 == 0){
            cout<< text <<endl;
            // cout<< "Frame No." << cnt <<"\t";
            // auto current_end = chrono::system_clock::now();
            // chrono::duration<double> current_elapsed_secs = current_end-current_begin;
            // cout<< "    Time elapsed: "<< current_elapsed_secs.count() << " s." <<endl;
        }
    }
    frame_log.close();
    detect_log.close();
    auto end = chrono::system_clock::now();
    chrono::duration<double> elapsed_secs = end - begin;
    cout<< "Average time elapsed per frame is " << elapsed_secs.count()/ image_names.size() << " s." <<endl;
    
    return 0;
}