#include <chrono>
#include <cstdint>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "odrive_can/msg/control_message.hpp"

using namespace std::chrono_literals;

/* This example creates a subclass of Node and uses a fancy C++11 lambda
* function to shorten the callback syntax, at the expense of making the
* code somewhat more difficult to understand at first glance. */

class SinePublisher : public rclcpp::Node
{
public:
  SinePublisher()
  : Node("sine_pub"), time_(0), vel(0.0f)
  {
    publisher_ = this->create_publisher<odrive_can::msg::ControlMessage>("/odrive_axis0/control_message", 10);
    auto timer_callback =
      [this]() -> void {
        vel =  sin(time_ * 0.1f) * 20.0f; 
        ++time_; //Increment time per loop every 100 ms
        //Calculate velocity as a sine wave with amplitude of 20 and frequency of 0.1 Hz

        odrive_can::msg::ControlMessage message;
        message.control_mode = 2;
        message.input_mode = 1;
        message.input_pos = 0.0f;
        message.input_vel = vel;
        message.input_torque = 0.0f;

        

        RCLCPP_INFO(
          this->get_logger(),
          "Publishing time=%llu control_mode=%u input_mode=%u input_pos=%.2f input_vel=%.2f input_torque=%.2f",
          static_cast<unsigned long long>(time_),
          message.control_mode,
          message.input_mode,
          message.input_pos,
          message.input_vel,
          message.input_torque);
        this->publisher_->publish(message);
      };
    timer_ = this->create_wall_timer(100ms, timer_callback);
  }

private:
  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Publisher<odrive_can::msg::ControlMessage>::SharedPtr publisher_;
  uint64_t time_;
  float vel;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<SinePublisher>());
  rclcpp::shutdown();
  return 0;
}
