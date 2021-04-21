#include <rclcpp/rclcpp.hpp>
#include <memory>
// MoveitCpp
#include <moveit/moveit_cpp/moveit_cpp.h>
#include <moveit/moveit_cpp/planning_component.h>

#include <geometry_msgs/msg/point_stamped.h>

/* #include <moveit_visual_tools/moveit_visual_tools.h>  This has not been ported to ros2 yet */
#include <rviz_visual_tools/rviz_visual_tools.hpp>
/* this is a standin for moveit_visual_tools visual_tools.prompt */
#include <moveit/macros/console_colors.h>
void prompt(const std::string& message)
{
  printf(MOVEIT_CONSOLE_COLOR_GREEN "\n%s" MOVEIT_CONSOLE_COLOR_RESET, message.c_str());
  fflush(stdout);
  while (std::cin.get() != '\n' && rclcpp::ok())
    ;
}

namespace rvt = rviz_visual_tools;

// All source files that use ROS logging should define a file-specific
// static const rclcpp::Logger named LOGGER, located at the top of the file
// and inside the namespace with the narrowest scope (if there is one)
static const rclcpp::Logger LOGGER = rclcpp::get_logger("moveit_cpp_tutorial");

int main(int argc, char** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::NodeOptions node_options;
  RCLCPP_INFO(LOGGER, "Initialize node");

  // This enables loading undeclared parameters
  // best practice would be to declare parameters in the corresponding classes
  // and provide descriptions about expected use
  node_options.automatically_declare_parameters_from_overrides(true);
  rclcpp::Node::SharedPtr node = rclcpp::Node::make_shared("run_moveit_cpp", "", node_options);

  // We spin up a SingleThreadedExecutor for the current state monitor to get information
  // about the robot's state.
  rclcpp::executors::SingleThreadedExecutor executor;
  executor.add_node(node);
  std::thread([&executor]() { executor.spin(); }).detach();

  // BEGIN_TUTORIAL
  //
  // Setup
  // ^^^^^
  //
  static const std::string PLANNING_GROUP = "panda_arm";
  static const std::string LOGNAME = "moveit_cpp_tutorial";

  /* Otherwise robot with zeros joint_states */
  rclcpp::sleep_for(std::chrono::seconds(1));

  RCLCPP_INFO(LOGGER, "Starting MoveIt Tutorials...");

  auto moveit_cpp_ptr = std::make_shared<moveit::planning_interface::MoveItCpp>(node);
  moveit_cpp_ptr->getPlanningSceneMonitor()->providePlanningSceneService();

  auto planning_components =
      std::make_shared<moveit::planning_interface::PlanningComponent>(PLANNING_GROUP, moveit_cpp_ptr);
  auto robot_model_ptr = moveit_cpp_ptr->getRobotModel();
  auto robot_start_state = planning_components->getStartState();
  auto joint_model_group_ptr = robot_model_ptr->getJointModelGroup(PLANNING_GROUP);

  // Visualization
  // ^^^^^^^^^^^^^
  //
  // The package MoveItVisualTools provides many capabilties for visualizing objects, robots,
  // and trajectories in RViz as well as debugging tools such as step-by-step introspection of a script
  rviz_visual_tools::RvizVisualTools visual_tools("panda_link0", "moveit_cpp_tutorial", node);
  /* moveit_visual_tools::MoveItVisualTools visual_tools("panda_link0", rvt::RVIZ_MARKER_TOPIC,
                                                         moveit_cpp_ptr->getPlanningSceneMonitor()); */
  visual_tools.deleteAllMarkers();
  /* visual_tools.loadRemoteControl(); */

  Eigen::Isometry3d text_pose = Eigen::Isometry3d::Identity();
  text_pose.translation().z() = 1.75;
  visual_tools.publishText(text_pose, "MoveItCpp Demo", rvt::WHITE, rvt::XLARGE);
  visual_tools.trigger();

  // Start the demo
  // ^^^^^^^^^^^^^^^^^^^^^^^^^
  prompt("Press 'Enter' to start the demo");
  /* visual_tools.prompt("Press 'next' in the RvizVisualToolsGui window to start the demo"); */

  // Planning with MoveItCpp
  // ^^^^^^^^^^^^^^^^^^^^^^^
  // There are multiple ways to set the start and the goal states of the plan
  // they are illustrated in the following plan examples
  //
  // Plan #1
  // ^^^^^^^
  //
  // We can set the start state of the plan to the current state of the robot
  planning_components->setStartStateToCurrentState();

  // The first way to set the goal of the plan is by using geometry_msgs::PoseStamped ROS message type as follow
  geometry_msgs::msg::PoseStamped target_pose1;
  target_pose1.header.frame_id = "panda_link0";
  target_pose1.pose.orientation.w = 1.0;
  target_pose1.pose.position.x = 0.28;
  target_pose1.pose.position.y = -0.2;
  target_pose1.pose.position.z = 0.5;
  planning_components->setGoal(target_pose1, "panda_link8");

  // Now, we call the PlanningComponents to compute the plan and visualize it.
  // Note that we are just planning
  auto plan_solution1 = planning_components->plan();

  // Check if PlanningComponents succeeded in finding the plan
  if (plan_solution1)
  {
    // Visualize the start pose in Rviz
    visual_tools.publishAxisLabeled(robot_start_state->getGlobalLinkTransform("panda_link8"), "start_pose");
    visual_tools.publishText(text_pose, "Start Pose", rvt::WHITE, rvt::XLARGE);
    // Visualize the goal pose in Rviz
    visual_tools.publishAxisLabeled(target_pose1.pose, "target_pose");
    visual_tools.publishText(text_pose, "Goal Pose", rvt::WHITE, rvt::XLARGE);
    // Visualize the trajectory in Rviz
    /* visual_tools.publishTrajectoryLine(plan_solution1.trajectory, joint_model_group_ptr); */
    visual_tools.trigger();

    /* Uncomment if you want to execute the plan */
    /* planning_components->execute(); // Execute the plan */
  }

  // Plan #1 visualization:
  //
  // .. image:: images/moveitcpp_plan1.png
  //    :width: 250pt
  //    :align: center
  //
  // Start the next plan
  visual_tools.deleteAllMarkers();
  prompt("Press 'Enter' to continue the demo");
  /* visual_tools.prompt("Press 'next' in the RvizVisualToolsGui window to continue the demo"); */

  // Plan #2
  // ^^^^^^^
  //
  // Here we will set the current state of the plan using
  // moveit::core::RobotState
  auto start_state = *(moveit_cpp_ptr->getCurrentState());
  geometry_msgs::msg::Pose start_pose;
  start_pose.orientation.w = 1.0;
  start_pose.position.x = 0.55;
  start_pose.position.y = 0.0;
  start_pose.position.z = 0.6;

  start_state.setFromIK(joint_model_group_ptr, start_pose);

  planning_components->setStartState(start_state);

  // We will reuse the old goal that we had and plan to it.
  auto plan_solution2 = planning_components->plan();
  if (plan_solution2)
  {
    moveit::core::RobotState robot_state(robot_model_ptr);
    moveit::core::robotStateMsgToRobotState(plan_solution2.start_state, robot_state);

    visual_tools.publishText(text_pose, "Start Pose", rvt::WHITE, rvt::XLARGE);
    visual_tools.publishAxisLabeled(robot_state.getGlobalLinkTransform("panda_link8"), "start_pose");
    visual_tools.publishText(text_pose, "Goal Pose", rvt::WHITE, rvt::XLARGE);
    visual_tools.publishAxisLabeled(target_pose1.pose, "target_pose");
    /* visual_tools.publishTrajectoryLine(plan_solution2.trajectory, joint_model_group_ptr); */
    visual_tools.trigger();

    /* Uncomment if you want to execute the plan */
    /* planning_components->execute(); // Execute the plan */
  }

  // Plan #2 visualization:
  //
  // .. image:: images/moveitcpp_plan2.png
  //    :width: 250pt
  //    :align: center
  //
  // Start the next plan
  visual_tools.deleteAllMarkers();
  prompt("Press 'Enter' to continue the demo");
  /* visual_tools.prompt("Press 'next' in the RvizVisualToolsGui window to continue the demo"); */

  // Plan #3
  // ^^^^^^^
  //
  // We can also set the goal of the plan using
  // moveit::core::RobotState
  auto target_state = *robot_start_state;
  geometry_msgs::msg::Pose target_pose2;
  target_pose2.orientation.w = 1.0;
  target_pose2.position.x = 0.55;
  target_pose2.position.y = -0.05;
  target_pose2.position.z = 0.8;

  target_state.setFromIK(joint_model_group_ptr, target_pose2);

  planning_components->setGoal(target_state);

  // We will reuse the old start that we had and plan from it.
  auto plan_solution3 = planning_components->plan();
  if (plan_solution3)
  {
    moveit::core::RobotState robot_state(robot_model_ptr);
    moveit::core::robotStateMsgToRobotState(plan_solution3.start_state, robot_state);

    visual_tools.publishText(text_pose, "Start Pose", rvt::WHITE, rvt::XLARGE);
    visual_tools.publishAxisLabeled(robot_state.getGlobalLinkTransform("panda_link8"), "start_pose");
    visual_tools.publishText(text_pose, "Goal Pose", rvt::WHITE, rvt::XLARGE);
    visual_tools.publishAxisLabeled(target_pose2, "target_pose");
    /* visual_tools.publishTrajectoryLine(plan_solution3.trajectory, joint_model_group_ptr); */
    visual_tools.trigger();

    /* Uncomment if you want to execute the plan */
    /* planning_components->execute(); // Execute the plan */
  }

  // Plan #3 visualization:
  //
  // .. image:: images/moveitcpp_plan3.png
  //    :width: 250pt
  //    :align: center
  //
  // Start the next plan
  visual_tools.deleteAllMarkers();
  prompt("Press 'Enter' to continue the demo");
  /* visual_tools.prompt("Press 'next' in the RvizVisualToolsGui window to continue the demo"); */

  // Plan #4
  // ^^^^^^^
  //
  // We can set the start state of the plan to the current state of the robot
  // We can set the goal of the plan using the name of a group states
  // for panda robot we have one named robot state for "panda_arm" planning group called "ready"
  // see `panda_arm.xacro
  // <https://github.com/ros-planning/panda_moveit_config/blob/melodic-devel/config/panda_arm.xacro#L13>`_

  /* // Set the start state of the plan from a named robot state */
  /* planning_components->setStartState("ready"); // Not implemented yet */
  // Set the goal state of the plan from a named robot state
  planning_components->setGoal("ready");

  // Again we will reuse the old start that we had and plan from it.
  auto plan_solution4 = planning_components->plan();
  if (plan_solution4)
  {
    moveit::core::RobotState robot_state(robot_model_ptr);
    moveit::core::robotStateMsgToRobotState(plan_solution4.start_state, robot_state);

    visual_tools.publishText(text_pose, "Start Pose", rvt::WHITE, rvt::XLARGE);
    visual_tools.publishAxisLabeled(robot_state.getGlobalLinkTransform("panda_link8"), "start_pose");
    visual_tools.publishText(text_pose, "Goal Pose", rvt::WHITE, rvt::XLARGE);
    visual_tools.publishAxisLabeled(robot_start_state->getGlobalLinkTransform("panda_link8"), "target_pose");
    /* visual_tools.publishTrajectoryLine(plan_solution4.trajectory, joint_model_group_ptr); */
    visual_tools.trigger();

    /* Uncomment if you want to execute the plan */
    /* planning_components->execute(); // Execute the plan */
  }

  // Plan #4 visualization:
  //
  // .. image:: images/moveitcpp_plan4.png
  //    :width: 250pt
  //    :align: center
  //
  // END_TUTORIAL
  visual_tools.deleteAllMarkers();
  prompt("Press 'Enter' to exit the demo");
  /* visual_tools.prompt("Press 'next' to end the demo"); */

  RCLCPP_INFO(LOGGER, "Shutting down.");
  rclcpp::shutdown();
  return 0;
}