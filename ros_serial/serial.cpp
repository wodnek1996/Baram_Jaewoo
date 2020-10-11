#include "ros/ros.h"
#include "std_msgs/String.h"
#include "std_msgs/Empty.h"
#include "geometry_msgs/Twist.h"
#include "move_base_msgs/MoveBaseActionResult.h"
#include "serial/serial.h"
#include "cstdlib"
#include "math.h"
#include "stdio.h"

static serial::Serial ser;



void write_callback(const std_msgs::String::ConstPtr& msg)
{
    ROS_INFO_STREAM("Writing to serial port" << msg->data);
    ser.write(msg->data);
}

void cmd_vel_callback(const geometry_msgs::Twist::ConstPtr& msg )
{
  double x;
  double z;
  char s1[17];
  char s2[17];

    x = msg->linear.x;
    z = msg->angular.z;
    ROS_INFO_STREAM("linear x " << msg->linear.x);
    ROS_INFO_STREAM("angular z " << msg->angular.z);
    sprintf(s1, "%lf", x);
    sprintf(s2, "%lf", z);
    ser.write("x : ");
    ser.write(s1);
    ser.write(" z : ");
    ser.write(s2);
    ser.write("\n\r ");
}
void result_callback(const move_base_msgs::MoveBaseActionResult::ConstPtr& msg)
{
    ROS_INFO_STREAM(msg->status.text);
    ser.write("Goal");
}
int main (int argc, char** argv)
{
    ros::init(argc, argv, "ros_serial_serial");
    ros::NodeHandle nh;

    ros::Subscriber write_sub = nh.subscribe("write", 1000, write_callback);
    ros::Publisher read_pub = nh.advertise<std_msgs::String>("read", 1000);

    ros::Subscriber goal_vel = nh.subscribe("cmd_vel",1000,cmd_vel_callback);
    ros::Subscriber result_sub = nh.subscribe("move_base/result",1000,result_callback);

    try
    {
        ser.setPort("/dev/ttyUSB0");
        ser.setBaudrate(115200);
        serial::Timeout to = serial::Timeout::simpleTimeout(1000);
        ser.setTimeout(to);
        ser.open();
    }
    catch (serial::IOException& e)
    {
        ROS_ERROR_STREAM("Unable to open port ");
        return -1;
    }

    if(ser.isOpen())
    {
        ROS_INFO_STREAM("Serial Port initialized");
    }
    else
    {
        return -1;
    }

    ros::Rate loop_rate(100);

    while(ros::ok())
    {

        ros::spinOnce();

        if(ser.available())
        {
            ROS_INFO_STREAM("Reading from serial port");
            std_msgs::String result;
            result.data = ser.read(ser.available());

            ROS_INFO_STREAM("Read: " << result.data);
            read_pub.publish(result);
        }
        loop_rate.sleep();
    }
}
