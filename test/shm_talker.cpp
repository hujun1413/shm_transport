#include "ros/ros.h"
#include <sstream>
#include "std_msgs/String.h"

#define MSGLEN (1024 * 1024 * 3)
#define HZ (60)

int main(int argc, char ** argv) {
  ros::init(argc, argv, "talker", ros::init_options::AnonymousName);
  ros::NodeHandle n;
  ros::Publisher p = n.advertise< std_msgs::String >("test_topic", HZ);

  sleep(1);
  ros::Rate loop_rate(HZ);
  int count = 0;
  while (ros::ok()) {
    std::stringstream ss;
    ss << "message #" << count << "...";
    std::string info = ss.str();

    for (int i = 0; i < MSGLEN - 20; i++)
        ss << " ";
    std_msgs::String msg;
    msg.data = ss.str();

    ROS_INFO("subscriber number: %d  info: [%s]", p.getNumSubscribers(), info.c_str());
    p.publish(msg);

    ros::spinOnce();
    loop_rate.sleep();
    count++;
  }
  return 0;
}
