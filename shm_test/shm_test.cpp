#include <gtest/gtest.h>
#include <ros/ros.h>
#include <std_msgs/String.h>
#include <sstream>
#include <shm_topic.hpp>

#define MSGLEN (1024 * 1024 * 3)
#define HZ (1)

std_msgs::String::ConstPtr str_received;
std_msgs::UInt64::ConstPtr uint64_received;

void strCallback(const std_msgs::String::ConstPtr & msg)
{
    str_received = msg;
}

void uint64Callback(const std_msgs::UInt64::ConstPtr & msg)
{
    uint64_received = msg;
}


TEST(ShmTester, stringTest)
{
    ros::NodeHandle n;
    shm_transport::Topic t(n);
    shm_transport::Subscriber< std_msgs::String > s = t.subscribe("shm_test_topic", 60, strCallback);
    shm_transport::Publisher p = t.advertise< std_msgs::String >("shm_test_topic", HZ, HZ * MSGLEN);

    ros::Rate loop_rate(HZ);
    int count = 0;
    while (ros::ok() && count <= 5)
    {
        std::stringstream ss;
        ss << "message #" << count << "...";
        for (int i = 0; i < MSGLEN - 20; i++)
          ss << " ";
        std_msgs::String msg;
        msg.data = ss.str();

        p.publish(msg);

        loop_rate.sleep();
        ros::spinOnce();

        count++;
        EXPECT_STREQ(msg.data.c_str(), str_received->data.c_str());
    }
}

TEST(ShmTester, uint64Test)
{
    ros::NodeHandle n;
    shm_transport::Topic t(n);
    shm_transport::Subscriber< std_msgs::UInt64 > s = t.subscribe("shm_test_topic", 60, uint64Callback);

    shm_transport::Publisher p = t.advertise< std_msgs::UInt64 >("shm_test_topic", HZ, HZ * 1000);

    ros::Rate loop_rate(HZ);
    int count = 0;
    while (ros::ok() && count <= 5)
    {
        std_msgs::UInt64 msg;
        msg.data = count;

        p.publish(msg);

        loop_rate.sleep();
        ros::spinOnce();

        count++;
        EXPECT_EQ(msg.data, uint64_received->data);
    }
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    ros::init(argc, argv, "shm_talker", ros::init_options::AnonymousName);

    return RUN_ALL_TESTS();
}
