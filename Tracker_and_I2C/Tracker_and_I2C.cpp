#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <cstring>

// the i2c dependencies
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

using namespace cv;
using namespace std;
 
// Convert to string
#define SSTR( x ) static_cast< std::ostringstream & >( \
( std::ostringstream() << std::dec << x ) ).str()

// The arduino board i2c address (slave)
#define ADDRESS 0x04

// The I2C bus: This is for V2 pi's. For V1 Model B you need i2c-0
static const char *devName = "/dev/i2c-1";

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
    
    // List of tracker types in OpenCV 3.2
    // NOTE : GOTURN implementation is buggy and does not work.
    string trackerTypes[6] = {"BOOSTING", "MIL", "KCF", "TLD","MEDIANFLOW", "GOTURN"};
    // vector <string> trackerTypes(types, std::end(types));
 
    // Create a tracker
    string trackerType = trackerTypes[4];
 
    Ptr<Tracker> tracker;
    // Tracker* tracker;
 
    #if (CV_MINOR_VERSION < 3)
    {
        tracker = Tracker::create(trackerType);
    }
    #else
    {
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
    }
    #endif
    // Read video
    // VideoCapture video("video-1509343693.mp4");
    VideoCapture video(0);
     
    // Exit if video is not opened
    if(!video.isOpened())
    {
        cout << "Could not read video file" << endl;
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
     
    // Uncomment the line below to select a different bounding box
    //bbox = selectROI(frame, false);
    
    double initx=bbox.x+bbox.width/2;
    double inity=bbox.y+bbox.height/2;
    double inith=bbox.height;

    // Display bounding box.
    rectangle(frame, bbox, Scalar( 255, 0, 0 ), 2, 1 );
    // imshow("Tracking", frame);
     
    tracker->init(frame, bbox);
   
    int FrameCounter=0;

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
        }
        else
        {
            // Tracking failure detected.
            // putText(frame, "Tracking failure detected", Point(100,80), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0,0,255),2);
            cout<<"\rTracking failure detected            ";
            cout.flush();
        }
         
        // Display tracker type on frame
        // putText(frame, trackerType + " Tracker", Point(100,20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50,170,50),2);
        
        // determine whether need move
        int Forward_Backward=0; // 0=stop, 1=forward, 2=backward;
        int Left_Right=0; // 0=stop, 1=left, 2=right
        double forward_backward_threshold=1.1;
        double right_left_threshold=1.15;
        if(bbox.height>(inith*forward_backward_threshold)) { // backward
            Forward_Backward=1; }
        else if(bbox.height<(inith/forward_backward_threshold)) { // forward
            Forward_Backward=2; }
        else { // stop
            Forward_Backward=0; }
        if(bbox.x+bbox.width/2>initx*right_left_threshold) { // turn right
            Left_Right=2; }
        else if(bbox.x+bbox.width/2<initx/right_left_threshold) { // turn left
            Left_Right=1; }
        else { // stop
            Left_Right=0; }

        // send the command through i2c
        unsigned char buffer[1];
        //Forward_Backward = 0;
	//Left_Right = 0;
        buffer[0] = Forward_Backward * 3 + Left_Right;
        if (write(file_i2c, buffer, 1) != 1)
        {
	    std::cout << "Failed to write to the i2c bus.";
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
