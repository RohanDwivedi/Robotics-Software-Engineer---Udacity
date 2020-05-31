#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>
#include<cmath>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
  // TODO: Request a service and pass the velocities to it to drive the robot
  
   ROS_INFO_STREAM("x: " + std::to_string(lin_x) + " z: " + std::to_string(ang_z));

  
  // call the drive robot service and pass the velocities to it
  ball_chaser::DriveToTarget srv;
  srv.request.linear_x = lin_x;
  srv.request.angular_z = ang_z;
  
  if(!client.call(srv))
  {ROS_ERROR("Failed to call service");}
}

// This callback function continuously executes and reads the image data
int pixel_col;
float lin_x,ang_z;

void process_image_callback(const sensor_msgs::Image img)
{
    // TODO: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera
  
	for (int i=0; i < img.height * img.step; i++)    
	{
		if (img.data[i] == 255 && img.data[i+1] ==255 and img.data[i+2] == 255)
        {
          // found the bright white one
          pixel_col = std::ceil(i%img.step);
          ROS_INFO_STREAM("img height: " + std::to_string(img.height) + " img.step: " + std::to_string(img.step) + "\n" + " col: " + std::to_string(pixel_col) + " i: " + std::to_string(i));
          break;
        }
      
       else
       { //STOP
         //ROS_INFO_STREAM("stop loop");
         lin_x = 0;
         ang_z = 0;
       }
    }
          
    if (pixel_col >= 0 && pixel_col < std::floor(img.step/3))
        { //LEFT
           ROS_INFO_STREAM("Left loop");
           lin_x = 0;
           ang_z = 0.5;
        }
          
    else if(pixel_col >= std::floor(img.step/3) && pixel_col <= std::floor(2*img.step/3))
        { //FORWARD
          ROS_INFO_STREAM("Forward loop");
          lin_x = 0.5;
          ang_z = 0;
        }
          
    else if (pixel_col > std::floor(2*img.step/3) && pixel_col <= img.step)
       { //RIGHT
          ROS_INFO_STREAM("Right loop");
          lin_x = 0;
          ang_z = -0.5;
       }
  
     drive_robot(lin_x, ang_z);
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
