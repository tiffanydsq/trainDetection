#include "opencv2/opencv.hpp"
#include <iostream>
// #include <highgui.h>

using namespace cv;
using namespace std;

int main(int argc,char **argv){
    if(argc!=5)
    {
        cout<<"Input format incorrect!"<<endl;
        cout<<"Usage: Input_video_path Output_video_path Frame_start Frame_end"<<endl;
	return -1;
    }
    string input_name = argv[1];
    string output_name = argv[2];
    int frame_start = atoi(argv[3]);
    int frame_end = atoi(argv[4]);
    VideoCapture capture(input_name);
    if (!capture.isOpened())
        {
            cerr << "Cannot initialize video!" << endl;
            return -1;
        }

    int scalar = 1;
    VideoWriter output_video(output_name,
    CV_FOURCC('M','J','P','G'), 
    capture.get(CV_CAP_PROP_FPS)*5, 
    Size(capture.get(CV_CAP_PROP_FRAME_WIDTH),capture.get(CV_CAP_PROP_FRAME_HEIGHT))
    );
    // namedWindow("Display frame",CV_WINDOW_NORMAL);
    // resizeWindow("Display frame", 600, 600);    
    int cnt=0;
    while(1){
        Mat frame;
        // Mat frame_mini;
        capture>>frame;
        cout<<cnt<<endl;
        if(cnt > frame_end || !frame.data){
            // cerr << "Error loading image!" << endl;
            break;
        }
            
        if (cnt < frame_start ){
            cnt++;
            continue;
        }
            
        // resize(frame, frame_mini, Size(frame.cols/scalar,frame.rows/scalar));
        // string index_frame = "No." + to_string(cnt);
        // putText(frame_mini, index_frame, Point(100/scalar, 200/scalar), FONT_HERSHEY_COMPLEX, 8.0/scalar, Scalar(0, 255, 0), 15/scalar);   
        cnt++;
        
        output_video<<frame;

        // imshow("Display frame", frame);

        // Esc key to stop 
        // if(waitKey(30)==27)
        //    break;

    }
    destroyAllWindows();
}
