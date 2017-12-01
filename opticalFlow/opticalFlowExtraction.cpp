#include <iostream>
#include <fstream>
// #include <vector>
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/video/tracking.hpp"
// #include <stdio.h>
#include <chrono>

using namespace cv;
using namespace std;
int main(int argc, const char** argv)
{
    // check command
    if (argc != 6)
    {
        cout << "Input format: ./opticalFlowExtraction input_image_folder RoI_mask_path output_folder Frame_start Frame_end" << endl;
        return -1;
    }
    bool flag_writevideo=true;
    int gridsize=20;
    int resize_scalar = 2;

    vector<String> image_names;
    glob(argv[1]+string("/*.jpg"), image_names);
    cout<<"input_image_folder: "<<argv[1]<<endl;
    Mat RoI_mask_full=imread(argv[2], 0);
    string output_video_path = argv[3] + string("/HOF.avi");
    int frame_start = atoi(argv[4]);
    int frame_end = atoi(argv[5]);

    int ROI_mask_offset_X = 0;
    int ROI_mask_offset_Y = 688;
    int ROI_mask_offset_width = 4112;
    int ROI_mask_offset_height = 1912;
    Mat RoI_mask;
    RoI_mask = RoI_mask_full(Rect( ROI_mask_offset_X, ROI_mask_offset_Y, ROI_mask_offset_width, ROI_mask_offset_height));
    

    cout<<"output_video_path: "<<output_video_path<<endl;
    Mat frame=imread(image_names[0]);
    VideoWriter video_flow(output_video_path,
        CV_FOURCC('M','J','P','G'), 
        5, 
        Size(frame.cols*2/resize_scalar,frame.rows/resize_scalar) 
    );

    Mat flow;
    UMat  flowUmat, prevgray;
   
    int key = 0;
    string image_name;
    // for(int cnt = 0; cnt < image_names.size(); cnt++){
    for(int cnt = frame_start; cnt < frame_end; cnt+=3){
        image_name = image_names[cnt];
        Mat img;
        Mat original;

        original=imread(image_name);
        original.copyTo(img);
        cvtColor(img, img, COLOR_BGR2GRAY);                          

        // vector<uchar> status;
        // vector<float> err;
        // vector<Point2f> points[2];
        // Size winSize(51,51);
        // TermCriteria termcrit(TermCriteria::COUNT|TermCriteria::EPS,20,0.03);
        if (!prevgray.empty()) {
            // calculate optical flow 
            calcOpticalFlowFarneback(prevgray, img, flowUmat, 0.3, 3, 20, 2, 5, 1.1, OPTFLOW_USE_INITIAL_FLOW);
            // calcOpticalFlowPyrLK(prevgray, img, points[0], points[1], status, err, winSize,
            //                      3, termcrit, 0, 0.001);
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
            cartToPolar	(OF[0], OF[1], magnitude, angle, true );	//angleInDegrees=true
            // cout<<"magnitude"<<magnitude.cols<<" "<<magnitude.rows<<endl;
            

            int hbins = 18;
            // float range[] = { Min, Max } ; //the upper boundary is exclusive
            // const float* histRange = { range };
            int idx_channel=0;
            // calcHist( &magnitude, 1, &idx_channel, Mat(), // do not use mask
            //     magnitude_hist, 1, &hbins, &histRange,
            //     true, // the histogram is uniform
            //     false );
            float range[] = { 0, 360 } ; //the upper boundary is exclusive
            const float* histRange = { range };
            Mat angle_hist;
            calcHist( &angle, 1, &idx_channel, RoI_mask, // do not use mask
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
            
            Mat merge(original.rows, original.cols + histImage.cols, CV_8UC3);
            // cout<<"merge"<<merge.cols<<" "<<merge.rows<<endl;
            // cout<<"original"<<original.cols<<" "<<original.rows<<endl;
            // cout<<"histImage"<<histImage.cols<<" "<<histImage.rows<<endl;

            Mat left(merge, Rect(0, 0, original.cols , original.rows));
            original.copyTo(left);
            Mat right(merge, Rect(original.cols, 0, histImage.cols, histImage.rows));
            histImage.copyTo(right);
            
            resize(merge, merge, Size(merge.cols/resize_scalar, merge.rows/resize_scalar));

            string index_frame = "Frame No." + to_string(cnt);
            putText(merge, index_frame, Point(100/resize_scalar, 200/resize_scalar), 
                    FONT_HERSHEY_COMPLEX, 8.0/resize_scalar, Scalar(0, 255, 0), 15/resize_scalar);   
        
            namedWindow("angle_hist", WINDOW_NORMAL );
            resizeWindow("angle_hist",900*2, 1200);
            imshow("angle_hist", merge);

            video_flow<<merge;     
            if (cvWaitKey(1)=='q') 
                break;
        }
        
        // fill previous image
        img.copyTo(prevgray);
        key = waitKey(20);
    }
}
