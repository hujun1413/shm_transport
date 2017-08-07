#include "ros/ros.h"
#include "std_msgs/String.h"
#include <sstream>
#include "shm_topic.hpp"

#define MSGLEN (1024 * 1024 * 3)
#define HZ (2)

int main(int argc, char ** argv) {
    ros::init(argc, argv, "shm_talker", ros::init_options::AnonymousName);
    ros::NodeHandle n;
    shm_transport::Topic t(n);
    std::vector<shm_transport::Publisher> vec(10);
    vec.resize(10);

    vec[1] = t.advertise< std_msgs::String >("shm_test_topic", HZ, (HZ+5) * MSGLEN); //take care the third parameter, should be big enough.

    shm_transport::Publisher p = vec[1];

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

        ROS_INFO("info: [%s]", info.c_str());

        p.publish(msg);

        ros::spinOnce();
        loop_rate.sleep();
        count++;
    }
    return 0;
}
