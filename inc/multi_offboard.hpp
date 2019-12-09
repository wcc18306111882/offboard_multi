//
// Created by zhouhua on 19-9-15.
//

#ifndef OFFBOARD_MULTI_OFFBOARD_HPP
#define OFFBOARD_MULTI_OFFBOARD_HPP

#include <geometry_msgs/PoseStamped.h>
#include <mavros_msgs/CommandBool.h>
#include <mavros_msgs/SetMode.h>
#include <mavros_msgs/State.h>
#include <mavros_msgs/VFR_HUD.h>
#include <mavros_msgs/PositionTarget.h>
#include <mavros_msgs/DebugValue.h>
#include <sensor_msgs/NavSatFix.h>
#include <geometry_msgs/TwistStamped.h>
#include "Multi_formation.hpp"
#include "FlightManager.hpp"
#include "Cinc.hpp"
#include "PathCreator.hpp"

using namespace std;
using namespace Eigen;
class MultiOffboard {
public:
    enum  {
        TAKEOFF,
        WAYPOINT,
        LAND,
        FALLOW_USV
    };

    enum {
        USA_INIT,
        USA_WAYPOINT,
        USA_DISARM
    };

    struct TVehicleMsg {
        int drone_id;
        mavros_msgs::State current_state;
        geometry_msgs::PoseStamped current_local_pos;
        geometry_msgs::PoseStamped target_pose;
        sensor_msgs::NavSatFix current_global_pos;

        ros::Subscriber state_sub;
        ros::Subscriber local_position_sub;
        ros::ServiceClient set_mode_client;
        ros::ServiceClient arming_client;
        ros::Publisher local_pos_pub;
        ros::Subscriber global_pos_sub;
    };

    MultiOffboard();

    ~MultiOffboard() = default;
    void vrf_hud_cb(const mavros_msgs::VFR_HUD::ConstPtr& msg);

    void uav1_global_pos_cb(const sensor_msgs::NavSatFix::ConstPtr& msg);
    void uav2_global_pos_cb(const sensor_msgs::NavSatFix::ConstPtr& msg);
    void uav3_global_pos_cb(const sensor_msgs::NavSatFix::ConstPtr& msg);
    void uav4_global_pos_cb(const sensor_msgs::NavSatFix::ConstPtr& msg);
    void uav5_global_pos_cb(const sensor_msgs::NavSatFix::ConstPtr& msg);
    void uav6_global_pos_cb(const sensor_msgs::NavSatFix::ConstPtr& msg);
    void uav7_global_pos_cb(const sensor_msgs::NavSatFix::ConstPtr& msg);

    void uav1_state_cb(const mavros_msgs::State::ConstPtr& msg);
    void uav2_state_cb(const mavros_msgs::State::ConstPtr& msg);
    void uav3_state_cb(const mavros_msgs::State::ConstPtr& msg);
    void uav4_state_cb(const mavros_msgs::State::ConstPtr& msg);
    void uav5_state_cb(const mavros_msgs::State::ConstPtr& msg);
    void uav6_state_cb(const mavros_msgs::State::ConstPtr& msg);
    void uav7_state_cb(const mavros_msgs::State::ConstPtr& msg);
    void uav1_local_pos_cb(const geometry_msgs::PoseStamped::ConstPtr& msg);
    void uav2_local_pos_cb(const geometry_msgs::PoseStamped::ConstPtr& msg);
    void uav3_local_pos_cb(const geometry_msgs::PoseStamped::ConstPtr& msg);
    void uav4_local_pos_cb(const geometry_msgs::PoseStamped::ConstPtr& msg);
    void uav5_local_pos_cb(const geometry_msgs::PoseStamped::ConstPtr& msg);
    void uav6_local_pos_cb(const geometry_msgs::PoseStamped::ConstPtr& msg);
    void uav7_local_pos_cb(const geometry_msgs::PoseStamped::ConstPtr& msg);

    void uav2_local_pos_sp_cb(const mavros_msgs::PositionTarget::ConstPtr& msg);
    void uav2_debug_value_cb(const mavros_msgs::DebugValue::ConstPtr& msg);

    bool pos_reached(geometry_msgs::PoseStamped &current_pos, geometry_msgs::PoseStamped &target_pos, float err_allow);
    void Oninit();
    void uav_targte_local_pos();
    void usv_targte_local_pos();
    void drone_pos_update();

    static  MultiOffboard* getInstance();

    mavros_msgs:: VFR_HUD current_vfr_hud;
    mavros_msgs::PositionTarget uav2_current_local_pos_sp;


    ros::NodeHandle nh;
    ros::Subscriber uav1_vfr_hud_sub;
    ros::Subscriber uav1_global_pos_sub;
    ros::Publisher uav1_g_speed_control_pub;

    ros::Subscriber uav2_local_pos_sp_sub;
    ros::Subscriber uav2_multi_formation_sub;
    ros::Subscriber uav2_global_pos_sub;


    TVehicleMsg drone_uav1_;
    TVehicleMsg drone_uav2_;
    TVehicleMsg drone_uav3_;
    TVehicleMsg drone_uav4_;
    TVehicleMsg drone_uav5_;
    TVehicleMsg drone_uav6_;
    TVehicleMsg drone_uav7_;

    bool is_offboard = false;
    bool is_armed = false;
    bool usv_armed = false;
    ros::Time last_request_;

private:
    static MultiOffboard* l_pInst;

    float curr_altitude;
    int uav_state_ = TAKEOFF;
    int usv_state_ = USA_INIT;
    geometry_msgs::PoseStamped target_pos_;
    vector<geometry_msgs::PoseStamped> uav_way_points;
    vector<geometry_msgs::PoseStamped> usv_way_points;

    bool usv5_reached_ = false;
    bool usv6_reached_ = false;
    bool usv7_reached_ = false;

    FlightManager::M_Drone m_drone_uav1_;
    FlightManager::M_Drone m_drone_uav2_;
    FlightManager::M_Drone m_drone_uav3_;
    FlightManager::M_Drone m_drone_uav4_;
    FlightManager::M_Drone m_drone_uav5_;
    FlightManager::M_Drone m_drone_uav6_;
    FlightManager::M_Drone m_drone_uav7_;
};

#endif //OFFBOARD_MULTI_OFFBOARD_HPP
