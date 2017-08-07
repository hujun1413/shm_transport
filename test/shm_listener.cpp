#include "ros/ros.h"
#include "std_msgs/String.h"
#include "shm_topic.hpp"

void chatterCallback(const std_msgs::String::ConstPtr & msg) {
    char str[21] = {'\0'};
    strncpy(str, msg->data.c_str(), 20);
    ROS_INFO("I heard: [%s]", str);
}

int main(int argc, char ** argv) {
    ros::init(argc, argv, "shm_listener", ros::init_options::AnonymousName);
    ros::NodeHandle n;
    shm_transport::Topic t(n);
    //shm_transport::Subscriber< std_msgs::String > s = t.subscribe("shm_test_topic", 60, chatterCallback);
    std::vector<shm_transport::Subscriber< std_msgs::String> > vec(3);
    vec[0] = t.subscribe< std_msgs::String >("shm_test_topic", 60, chatterCallback);
    //vec[1] = t.subscribe< std_msgs::String >("shm_test_topic", 60, chatterCallback);

    //vec.push_back(t.subscribe< std_msgs::String >("shm_test_topic", 60, chatterCallback));
    //vec.push_back(t.subscribe< std_msgs::String >("shm_test_topi", 60, chatterCallback));
    ros::spin();
    return 0;
}
