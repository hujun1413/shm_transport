#include <gtest/gtest.h>
#include <ros/ros.h>
#include <std_msgs/String.h>
#include <sstream>
#include <shm_topic.hpp>

#define MSGLEN (1024 * 1024 * 3)
#define HZ (1)

std::string str_send;
uint64_t uint64_send;

static int count = 0;
void strCallback(const std_msgs::String::ConstPtr & msg)
{
    std::stringstream ss;
    ss << "message #" << count << "...";
    for (int i = 0; i < MSGLEN - 20; i++)
    ss << " ";

    EXPECT_STREQ(ss.str().c_str(), msg->data.c_str());
    ++count;
}

void uint64Callback(const std_msgs::UInt64::ConstPtr & msg)
{
    EXPECT_EQ(count, msg->data);
    ++count;
}


/*TEST(ShmTester, stringTest)
{
    pid_t child;
    if((child = fork()) == -1)
    {
       perror("fork error\n");
       exit(EXIT_FAILURE);
    }
    else if(child == 0)     //in child process
    {
        printf("child1 %d\n", getpid());
        int argc = 1;
        char * argv[] = {(char *)"shm_talker", NULL};
        ros::init(argc, (char **)argv, "shm_talker", ros::init_options::AnonymousName);
        ros::NodeHandle n;
        shm_transport::Topic t(n);
        shm_transport::Publisher p = t.advertise< std_msgs::String >("shm_test_topic", HZ, (HZ+1) * MSGLEN);
        sleep(1);
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

            str_send = msg.data;
            p.publish(msg);

            ros::spinOnce();
            loop_rate.sleep();

            count++;
        }
        ros::shutdown();
        printf("-222222222222222222222222222hahahha\n");
    }
    else
    {
        printf("father1 %d\n", getpid());
        int argc = 1;
        char * argv[] = {(char *)"shm_listener", NULL};
        ros::init(argc, (char **)argv, "shm_listener", ros::init_options::AnonymousName);
        ros::NodeHandle n;
        shm_transport::Topic t(n);
        shm_transport::Subscriber< std_msgs::String > s = t.subscribe("shm_test_topic", 60, strCallback);
        ros::Rate loop_rate(60);
        while(ros::ok && count <= 5)
        {
            //ROS_INFO("%d",count);
            loop_rate.sleep();
            ros::spinOnce();
        }
        ros::shutdown();

        wait(NULL);
        printf("------------------------hahahha\n");
    }
}*/

TEST(ShmTester, uint64Test)
{
    pid_t child;
    if((child = fork()) == -1)
    {
        perror("fork error\n");
        exit(EXIT_FAILURE);
    }
    else if(child == 0)     //in child process
    {
        printf("child2 %d\n", getpid());
        int argc = 1;
        char * argv[] = {(char *)"shm_talker", NULL};
        ros::init(argc, (char **)argv, "shm_talker", ros::init_options::AnonymousName);
        ros::NodeHandle n;
        shm_transport::Topic t(n);
        shm_transport::Publisher p = t.advertise< std_msgs::UInt64 >("/shm_test_topic1", HZ, HZ * 1000);
        sleep(1);
        ros::Rate loop_rate(HZ);
        int count = 0;
        while (ros::ok() && count <= 5)
        {
            std_msgs::UInt64 msg;
            msg.data = count;

            uint64_send = msg.data;
            p.publish(msg);

            loop_rate.sleep();
            ros::spinOnce();

            count++;
        }
        ros::shutdown();
        printf("-111111111111111111111111111hahahha\n");

    }
    else
    {
        printf("father2 %d\n", getpid());
        int argc = 1;
        char * argv[] = {(char *)"shm_listener", NULL};
        ros::init(argc, (char **)argv, "shm_listener", ros::init_options::AnonymousName);
        ros::NodeHandle n;
        shm_transport::Topic t(n);
        shm_transport::Subscriber< std_msgs::UInt64 > s = t.subscribe("/shm_test_topic1", 60, uint64Callback);
        ros::Rate loop_rate(60);
        while(ros::ok && count <= 5)
        {
            //ROS_INFO("%d",count);
            loop_rate.sleep();
            ros::spinOnce();
        }
        ros::shutdown();

        pid_t wait = waitpid(child, NULL, WNOHANG);
        while(wait == 0)
        {
            printf("i am waitiong for child process\n");
            sleep(1);
            wait = waitpid(child, NULL, WNOHANG);
        }
        if(wait == child)
        {
            printf("i am father process, exiting\n");
        }
        printf("-**************************hahahha\n");
    }
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    //ros::init(argc, argv, "shm_test", ros::init_options::AnonymousName);

    return RUN_ALL_TESTS();
}
