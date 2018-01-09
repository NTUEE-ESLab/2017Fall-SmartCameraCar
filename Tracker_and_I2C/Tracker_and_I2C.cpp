#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/dnn/shape_utils.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <cstring>
#include <fstream>
#include <algorithm>
#include <cstdlib>

// the i2c dependencies
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

using namespace cv;
using namespace std;
using namespace cv::dnn;

// Convert to string
#define SSTR( x ) static_cast< std::ostringstream & >( \
( std::ostringstream() << std::dec << x ) ).str()

// The arduino board i2c address (slave)
#define ADDRESS 0x04

// The I2C bus: This is for V2 pi's. For V1 Model B you need i2c-0
static const char *devName = "/dev/i2c-1";

bool Target_Detector(string& TargetClass, Rect2d& bbox, Mat& frame, dnn::Net& net, vector<String>& classNamesVec)
{
    if (frame.channels() == 4)
        cvtColor(frame, frame, COLOR_BGRA2BGR);
    Mat inputBlob = blobFromImage(frame, 1 / 255.F, Size(416, 416), Scalar(), true, false); //Convert Mat to batch of images
    net.setInput(inputBlob, "data");                   //set the network input
    Mat detectionMat = net.forward("detection_out");   //compute output
    // float confidenceThreshold = parser.get<float>("min_confidence");

    bool detected=false;
    float max_condidence=0; // the confidence of detecting certain terget

    for (int i = 0; i < detectionMat.rows; i++)
    {
        const int probability_index = 5;
        const int probability_size = detectionMat.cols - probability_index;
        float *prob_array_ptr = &detectionMat.at<float>(i, probability_index);
        size_t objectClass = max_element(prob_array_ptr, prob_array_ptr + probability_size) - prob_array_ptr;
        float confidence = detectionMat.at<float>(i, (int)objectClass + probability_index);
        // if (confidence > confidenceThreshold)
        if(confidence>0.1)
        {
            float x_center = detectionMat.at<float>(i, 0) * frame.cols;
            float y_center = detectionMat.at<float>(i, 1) * frame.rows;
            float width = detectionMat.at<float>(i, 2) * frame.cols;
            float height = detectionMat.at<float>(i, 3) * frame.rows;
            Point p1(cvRound(x_center - width / 2), cvRound(y_center - height / 2));
            Point p2(cvRound(x_center + width / 2), cvRound(y_center + height / 2));
            Rect object(p1, p2);
            Scalar object_roi_color(0, 255, 0);
            // if (object_roi_style == "box")
            // {
                rectangle(frame, object, object_roi_color);
            // }
            // else
            // {
            //     Point p_center(cvRound(x_center), cvRound(y_center));
            //     line(frame, object.tl(), p_center, object_roi_color, 1);
            // }
            String className = objectClass < classNamesVec.size() ? classNamesVec[objectClass] : cv::format("unknown(%d)", objectClass);
            String label = format("%s: %.2f", className.c_str(), confidence);
            int baseLine = 0;
            Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
            rectangle(frame, Rect(p1, Size(labelSize.width, labelSize.height + baseLine)),
                      object_roi_color, CV_FILLED);
            putText(frame, label, p1 + Point(0, labelSize.height),
                    FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0,0,0));
            cout<<label<<", x="<<x_center<<", y="<<y_center<<", h="<<height<<", w="<<width<<endl;

            if(className==TargetClass) // choose the init bbox
            {
                detected=true;
                if(confidence>max_condidence)
                {
                    max_condidence=confidence;
                    bbox.x=x_center-width/2;
                    bbox.y=y_center-height/2;
                    bbox.height=height;
                    bbox.width=width;
                }
            }
        }
    }
    imshow("YOLO: Detections", frame);
    // while(1) {
    //     if(waitKey(1)==27) break; }
    return detected;
}

int main(int argc, char **argv)
{

    // Initiate i2c setting
    // Open the i2c bus
    int file_i2c;
    if ((file_i2c = open(devName, O_RDWR)) < 0)
    {
        std::cout << "Failed to open the i2c bus\n";
    return 0;
    }
    
    // Set the I2C address of the slave
    int addr = 0x04;
    if (ioctl(file_i2c, I2C_SLAVE, addr) < 0)
    {
        std::cout << "Failed to acquire bus access and/or talk to slave..\n";
        return 0;
    }

    // load the yolo detector NN
    dnn::Net net = readNetFromDarknet("../yolo_data/tiny-yolo-voc.cfg", "../yolo_data/tiny-yolo-voc.weights");
    if (net.empty()) { cerr<<"Cannot Load network file!\n"; exit(-1); }

    // load the yolo class name
    vector<String> classNamesVec;
    ifstream classNamesFile("../yolo_data/voc.names");
    if (classNamesFile.is_open())
    {
        string className = "";
        while (std::getline(classNamesFile, className))
            classNamesVec.push_back(className);
    }
    else { cerr<<"Cannot Load class name!\n"; exit(-1); }

    // List of tracker types in OpenCV 3.3
    // NOTE : GOTURN implementation is buggy and does not work.
    string trackerTypes[6] = {"BOOSTING", "MIL", "KCF", "TLD","MEDIANFLOW", "GOTURN"};
    // vector <string> trackerTypes(types, std::end(types));
 
    // Create a tracker
    string trackerType = trackerTypes[4];
 
    Ptr<Tracker> tracker;
 
    if (trackerType == "BOOSTING")
        tracker = TrackerBoosting::create();
    if (trackerType == "MIL")
        tracker = TrackerMIL::create();
    if (trackerType == "KCF")
        tracker = TrackerKCF::create();
    if (trackerType == "TLD")
        tracker = TrackerTLD::create();
    if (trackerType == "MEDIANFLOW")
        tracker = TrackerMedianFlow::create();
    if (trackerType == "GOTURN")
        tracker = TrackerGOTURN::create();
    
    // Read video
    // VideoCapture video("video-1509343693.mp4");
    VideoCapture video(0);
     
    // Exit if video is not opened
    if(!video.isOpened())
    {
        cerr << "Could not read video file" << endl;
        return 1;         
    }

    video.set(3,320);
    video.set(4,240);
    // video.set(5,30);
     
    // Read first frame
    Mat frame;
    bool ok = video.read(frame);
     
    // Define initial boundibg box
    Rect2d bbox(120, 80, 86, 80);
     
    // Manual select a bounding box
    // bbox = selectROI(frame, false);

    // use detection to select a bounding box
    string targetclass="person";
    Target_Detector(targetclass,bbox,frame,net,classNamesVec);

    
    double initx=bbox.x+bbox.width/2;
    double inity=bbox.y+bbox.height/2;
    double inith=bbox.height;

    // Display bounding box.
    rectangle(frame, bbox, Scalar( 255, 0, 0 ), 2, 1 );
    // imshow("Tracking", frame);
     
    tracker->init(frame, bbox);

    cout<<"inti bbox, x="<<bbox.x<<", y="<<bbox.y<<", h="<<bbox.height<<", w="<<bbox.width<<endl;
   
    int FrameCounter=0;

    // for tracking movement
    int Forward_Backward=0; // 0=stop, 1=forward, 2=backward;
    int Left_Right=0; // 0=stop, 1=left, 2=right
    double forward_backward_threshold=1.1;
    double right_left_threshold=1.15;

    // while(video.read(frame))    
    while(1)
    { 
        video>>frame;
        // Start timer
        double timer = (double)getTickCount();

        // Update the tracking result
        bool ok = tracker->update(frame, bbox);
        // tracker->update(frame, bbox);;

        // Calculate Frames per second (FPS)
        float fps = getTickFrequency() / ((double)getTickCount() - timer);
        
        if (ok)
        {
            // Tracking success : Draw the tracked object
            rectangle(frame, bbox, Scalar( 255, 0, 0 ), 2, 1 );
            cout<<"\rfps="<<int(fps)<<" x="<<int(bbox.x+bbox.width/2)<<" y="<<int(bbox.y+bbox.height/2)<<" htight="<<int(bbox.height)<<" width="<<int(bbox.width)<<"     ";
            cout.flush();

            // determine whether need move
            if(bbox.height>(inith*forward_backward_threshold)) { // backward
                Forward_Backward=2; }
            else if(bbox.height<(inith/forward_backward_threshold)) { // forward
                Forward_Backward=1; }
            else { // stop
                Forward_Backward=0; }
            if(bbox.x+bbox.width/2>initx*right_left_threshold) { // turn right
                Left_Right=2; }
            else if(bbox.x+bbox.width/2<initx/right_left_threshold) { // turn left
                Left_Right=1; }
            else { // stop
                Left_Right=0; }
        }
        else
        {
            // Tracking failure detected.
            // putText(frame, "Tracking failure detected", Point(100,80), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0,0,255),2);
            cout<<"\rTracking failure detected            ";
            cout.flush();

            // Tell arduino not to move
            Left_Right=0;
            Forward_Backward=0;

            if(Target_Detector(targetclass, bbox, frame, net, classNamesVec))
            {
                tracker->clear();
                Ptr<Tracker> NewTracker = TrackerMedianFlow::create();
                tracker=NewTracker;
                tracker->init(frame,bbox);
                cout<<"trakcer re-inited\n";
            }
        }
         
        // Display tracker type on frame
        // putText(frame, trackerType + " Tracker", Point(100,20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50,170,50),2);

        // send the command through i2c
        unsigned char buffer[1];
        //Forward_Backward = 0;
        //Left_Right = 0;
        buffer[0] = Forward_Backward * 3 + Left_Right;
        std::cout << Forward_Backward << " " << Left_Right;
        cout.flush();
        if (write(file_i2c, buffer, 1) != 1)
        {
            cout << "Failed to write to the i2c bus.";
        }
        else
        {
            cout << "                               ";
        }
            
        FrameCounter++;

        if(FrameCounter%45==0)
        {
            // Display FPS on frame
            putText(frame, "FPS : " + SSTR(int(fps)), Point(100,50), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50,170,50), 2);

            // Display frame.
            imshow("Tracking", frame);
        }
         
        // Exit if ESC pressed.
        int k = waitKey(1);
        if(k == 27)
        {
            buffer[0] = 0;
            if (write(file_i2c, buffer, 1) != 1)
            {
                std::cout << "Failed to write to the i2c bus.";
            }   
            cout<<endl;
            break;
        }
    }
    return 0;
}
