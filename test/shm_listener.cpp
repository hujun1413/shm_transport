#include "ros/ros.h"
#include "std_msgs/String.h"

void chatterCallback(const std_msgs::String::ConstPtr & msg) {
  char str[21] = {'\0'};
  strncpy(str, msg->data.c_str(), 20);
  ROS_INFO("I heard: [%s]", str);
}

int main(int argc, char ** argv) {
  ros::init(argc, argv, "listener", ros::init_options::AnonymousName);
  ros::NodeHandle n;
  ros::Subscriber s = n.subscribe("test_topic", 60, chatterCallback);
  ros::Subscriber s1 = n.subscribe("test_topic", 60, chatterCallback);
  ros::spin();
  return 0;
}
