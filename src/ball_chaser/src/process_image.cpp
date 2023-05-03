#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{    
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;
    
    if(client.call(srv)){
        ROS_INFO("DriveToTarget service called successfully");
    }
    else{
        ROS_ERROR("Failed to call service DriveToTarget service");
    }
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;
    int left_boundary = img.step / 3;
    int right_boundary = 2 * left_boundary;
    // int ball_position = -1;
    // int ball_pixels = 0;
    bool ball_found = false;

    // ROS_INFO("BOUNDARIES: [%d, %d]", left_boundary, right_boundary);

    for (int i = 0; i < img.height * img.step; i+= 3){        // for(int i=0; i<img.height*img.step; i+=3)

        int r = img.data[i];
        int g = img.data[i+1];
        int b = img.data[i+2];
        // ROS_INFO("pixel_value : %d", pixel_value);
        
        if(r == white_pixel && g == white_pixel && b == white_pixel){
            // ROS_INFO("white pixel found");
            // divide the index (column position)by 3 to get the column position of the current pixel in the image
            int col = i % img.step;               // int col = (i % img.step) / 3;
            // ROS_INFO("col : %d", col);                   
            // ROS_INFO("ball position: %d", ball_position);
            if(col < left_boundary){
                drive_robot(0.5, 1.0);
            }
            else if(col < right_boundary){
                drive_robot(0.5, 0);
            }
            else{
                drive_robot(0.5, -1.0);
            }
            ball_found = true;
            break;        
        }
    }
    if(ball_found == false){
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