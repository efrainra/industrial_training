/*
 * open_gripper.cpp
 *
 *  Created on: Jun 3, 2013
 *      Author: ros-industrial
 */

#include <collision_avoidance_pick_and_place/pick_and_place.h>

/*    SET ATTACHED OBJECT
  Goal:
    - Attaches or detaches target to arms end-effector link.
    - Publishes object marker for visualization.
  Hints:
*/

namespace collision_avoidance_pick_and_place
{
void PickAndPlace::set_attached_object(bool attach, const geometry_msgs::Pose &pose)
{
  //ROS_ERROR_STREAM("set_attached_object is not implemented yet.  Aborting."); exit(1);

  // adding attached box to collision matrix
	robot_model_loader::RobotModelLoader robot_model_loader("robot_description");
	robot_model::RobotModelPtr kinematic_model = robot_model_loader.getModel();
	planning_scene::PlanningScene planning_scene(kinematic_model);
	planning_scene.setCurrentState(*move_group_ptr->getCurrentState());
	collision_detection::AllowedCollisionMatrix &acm = planning_scene.getAllowedCollisionMatrixNonConst();

	// modifying allowed collision matrix
	acm.setDefaultEntry(cfg.ATTACHED_OBJECT_LINK_NAME,!attach);
	acm.setEntry(cfg.ATTACHED_OBJECT_LINK_NAME,"<octomap>",!attach);
	for(unsigned int i = 0;i < cfg.TOUCH_LINKS.size();i++)
	{
		acm.setEntry(cfg.ATTACHED_OBJECT_LINK_NAME,cfg.TOUCH_LINKS[i],true);
	}

	// create planning scene message
	moveit_msgs::PlanningScene planning_scene_msg;
	planning_scene.getPlanningSceneMsg(planning_scene_msg);
	planning_scene_msg.is_diff = true;
	planning_scene_msg.world = moveit_msgs::PlanningSceneWorld();


	// updating orientation
	geometry_msgs::Quaternion q = pose.orientation;
	q.x = -q.x;
	q.y = -q.y;
	q.z = -q.z;
	q.w = 1;
	cfg.MARKER_MESSAGE.pose.orientation = q;

	// updating marker action
	cfg.MARKER_MESSAGE.action =
			attach ? visualization_msgs::Marker::ADD : visualization_msgs::Marker::DELETE;

	// publish messages
	marker_publisher.publish(cfg.MARKER_MESSAGE);
	planning_scene_publisher.publish(planning_scene_msg);
	ros::Duration(1.0f).sleep();

}

}


