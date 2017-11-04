/* find train region mask */

#include "opencv2/opencv.hpp"
#include <stdio.h>

using namespace cv;
using namespace std;

// take as input the video path, the starting frame index and ending frame index
int main(int argc,char **argv){

    string input_name=argv[1];
    
    VideoCapture capture(input_name);
   
    if (!capture.isOpened())
    {
        cerr << "Cannot initialize video!" << endl;
        return -1;
    }
    
    int start_frame=atoi(argv[3]);
    int end_frame=atoi(argv[4]);
    int num_frame=capture.get(CAP_PROP_FRAME_COUNT);
    Mat output_mask;
    int key = 0;
    int cnt=0;
    while (key != 'q')
    {
        Mat img_input;
        capture >> img_input;
        cnt++;
        
        if(!img_input.data)
            cerr << "Error loading image!" << endl;

        if (cnt>start_frame)
        {
            img_input.convertTo(img_input, CV_32F);
            output_mask=output_mask+img_input/(end_frame-start_frame);
        }
        
        

        if(cnt>end_frame) 
            break;
        // imshow("Display frame", img_input);

        key = cvWaitKey(33);
    }
    output_mask.convertTo(output_mask, CV_8UC3);
    cvtColor(output_mask,output_mask,CV_RGB2GRAY);    
    threshold(output_mask, output_mask,0, 255, THRESH_OTSU);
    imwrite(argv[2],output_mask);
    capture.release();
    destroyAllWindows();
}
