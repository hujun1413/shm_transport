#ifndef __SHM_PUBLISHER_HPP__
#define __SHM_PUBLISHER_HPP__

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/atomic/atomic.hpp>
#include "ros/ros.h"

namespace shm_transport {

    class Topic;

    class Publisher {
    private:
        Publisher(const ros::Publisher & pub_, const std::string & topic, uint32_t mem_size) {
            impl_ = boost::make_shared<Impl>();
            printf("in advertise\n");
            printf("impl %p count : %d\n", impl_.get(), (int)impl_.use_count());
            impl_->pub = boost::make_shared< ros::Publisher >(pub_);
            impl_->pshm = new boost::interprocess::managed_shared_memory(boost::interprocess::open_or_create, topic.c_str(), mem_size);
            boost::atomic<uint32_t> *ref_ptr = impl_->pshm->find_or_construct<boost::atomic<uint32_t> >("ref")(0);
            ref_ptr->fetch_add(1, boost::memory_order_relaxed);
        }

        class Impl {
        public:
            Impl() {
                printf("in impl construct\n");
                printf("impl %p\n", this);
                pshm = NULL;
            }

            ~Impl() {
                printf("in impl disconstruct0%p %p\n", pshm, pub.get());
                if (pshm) {
                    boost::atomic<uint32_t> * ref_ptr = pshm->find_or_construct<boost::atomic<uint32_t> >("ref")(0);
                    printf("in impl disconstruct1\n");
                    if (ref_ptr->fetch_sub(1, boost::memory_order_relaxed) == 1) {
                        printf("in impl disconstruct2\n");
                        if(pub)
                        {
                            boost::interprocess::shared_memory_object::remove(pub->getTopic().c_str());
                            printf("shm file <%s>　removed\n", pub->getTopic().c_str());
                        }
                        else
                        {
                            printf("no pub in pshm\n");
                        }
                    }
                    delete pshm;
                }
                if (pub) {
                    pub->shutdown();
                }
                printf("in impl disconstruct\n");
            }

            boost::shared_ptr< ros::Publisher > pub;
            boost::interprocess::managed_shared_memory * pshm;
        };

        boost::shared_ptr<Impl> impl_;

    public:
        Publisher() {
            printf("default construct\n");
            impl_ = boost::make_shared<Impl>();
            printf("impl %p count : %d\n", impl_.get(), (int)impl_.use_count());
        }

        Publisher(const Publisher& rhs) {
            printf("copy construct\n");
            impl_ = rhs.impl_;
            printf("impl %p count : %d\n", impl_.get(), (int)impl_.use_count());
        }

        ~Publisher() {
            printf("disconstruct\n");
            printf("impl %p count : %d\n", impl_.get(), (int)impl_.use_count());
        }

        template < class M >
        void publish(const M & msg) const {
            if (!impl_->pshm)
                return;
            if (impl_->pub->getNumSubscribers() == 0)
                return;

            uint32_t serlen = ros::serialization::serializationLength(msg);
            uint32_t * ptr = (uint32_t *)impl_->pshm->allocate(sizeof(uint32_t) * 2 + serlen);
            ptr[0] = impl_->pub->getNumSubscribers();
            ptr[1] = serlen;
            ros::serialization::OStream out((uint8_t *)(ptr + 2), serlen);
            ros::serialization::serialize(out, msg);

            std_msgs::UInt64 actual_msg;
            actual_msg.data = impl_->pshm->get_handle_from_address(ptr);
            impl_->pub->publish(actual_msg);
        }

        void shutdown() {
            impl_->pub->shutdown();
        }

        std::string getTopic() const {
            return impl_->pub->getTopic();
        }

        uint32_t getNumSubscribers() const {
            return impl_->pub->getNumSubscribers();
        }

    protected:


        friend class Topic;
    };
}

#endif // __SHM_PUBLISHER_HPP__

