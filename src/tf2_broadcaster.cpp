#include <geometry_msgs/msg/transform_stamped.hpp>

#include <rclcpp/rclcpp.hpp>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2_ros/transform_broadcaster.h>
#include <geometry_msgs/msg/transform.h>

#include <memory>
#include <string>

using std::placeholders::_1;

class FramePublisher : public rclcpp::Node
{
public:
  FramePublisher()
  : Node("tf2_frame_publisher")
  {
    // Initialize the transform broadcaster
    tf_broadcaster_ =
      std::make_unique<tf2_ros::TransformBroadcaster>(*this);

    subscription_ = this->create_subscription<geometry_msgs::msg::Transform>(
      "/encoder_data", 10,
      std::bind(&FramePublisher::handle_transform, this, _1));
  }

private:
  void handle_transform(const std::shared_ptr<const geometry_msgs::msg::Transform> msg)
  {
    rclcpp::Time now = this->get_clock()->now();
    geometry_msgs::msg::TransformStamped t;

    // Read message content and assign it to
    // corresponding tf variables
    t.header.stamp = now;
    t.header.frame_id = "odom";
    t.child_frame_id = "base_footprint";

    // Turtle only exists in 2D, thus we get x and y translation
    // coordinates from the message and set the z coordinate to 0
    t.transform.translation.x = msg->translation.x;
    t.transform.translation.y = msg->translation.y;
    t.transform.translation.z = 0.0;

    // For the same reason, turtle can only rotate around one axis
    // and this why we set rotation in x and y to 0 and obtain
    // rotation in z axis from the message

    t.transform.rotation = msg->rotation;

    // Send the transformation
    tf_broadcaster_->sendTransform(t);
  }
  rclcpp::Subscription<geometry_msgs::msg::Transform>::SharedPtr subscription_;
  std::unique_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<FramePublisher>());
  rclcpp::shutdown();
  return 0;
}


