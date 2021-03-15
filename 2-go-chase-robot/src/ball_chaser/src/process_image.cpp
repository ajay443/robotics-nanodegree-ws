#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
    ROS_INFO_STREAM("Sending Drive parameters");

    // Create a request
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    // Call the command_robot service with request values given above
    if (!client.call(srv))
        ROS_ERROR("Failed to call service /ball_chaser/command_robot");
}

// This callback function continuously executes and reads the image data
// Image datatype is defined in ROS - http://docs.ros.org/en/api/sensor_msgs/html/msg/Image.html
void process_image_callback(const sensor_msgs::Image img)
{
    ROS_INFO_STREAM("Processing the Image");
    auto drive = false;
    // Image data --> uint8[] data          # actual matrix data, size is (step * rows)
    int white_pixel = 255;
    for (int i = 0; i < img.height * img.step; i+=3) { 
        if (img[i] == white_pixel && img[i+1] == white_pixel && img[i+2] == white_pixel) {
            auto col = i % img.step;
            if (col < img.step * 0.4) {
                drive_robot(0.5, 1.0);          // Left
            }else if (col > img.step * 0.6) {
                drive_robot(0.5, -1.0);         // Right
            }else {
                drive_robot(0.5, 0.0);         // mid 
            }
            drive = true;
            break;
        }
    }
    if(!drive) {
        drive_robot(0.0, 0.0);
    }
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
