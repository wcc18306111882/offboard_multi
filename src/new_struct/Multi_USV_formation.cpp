//
// Created by zhouhua on 19-12-1.
//

#include <geometry_msgs/PoseStamped.h>
#include <DataMan.hpp>
#include <Calculate.hpp>
#include "Multi_USV_formation.hpp"

MultiUSVFormation* MultiUSVFormation::multi_formation = NULL;

MultiUSVFormation::MultiUSVFormation() :
        is_formation_(false),
        is_get_takeoff_pos_(false),
        usv1_reached_(false),
        usv2_reached_(false),
        usv3_reached_(false){

}

void MultiUSVFormation::Oninit(const int config_) {
    util_log("usv formation config = %d", config_ );
    leader_curr_pos_ = TVec3(m_multi_vehicle_.usv1.current_local_pos.pose.position.x, m_multi_vehicle_.usv1.current_local_pos.pose.position.y,
                            m_multi_vehicle_.usv1.current_local_pos.pose.position.z);
    switch (config_) {
        case VF_USV_TRIANGLE: {
            is_formation_ = true;
            util_log("usv Formation call! Triangle!");
            leader_drone_ = m_multi_vehicle_.usv1;
            Drone_usv2_ = TVec3(-K_multi_usv_formation_distance, -K_multi_usv_formation_distance , m_multi_vehicle_.usv2.current_local_pos.pose.position.z);
            Drone_usv3_ = TVec3(K_multi_usv_formation_distance, -K_multi_usv_formation_distance , m_multi_vehicle_.usv3.current_local_pos.pose.position.z);
            calcFollowUSVPos();

        }
            break;


        case VF_USV_LINE_HORIZONTAL : {
            is_formation_ = true;
            util_log("usv Formation call! Line horizontal!");
            leader_drone_ = m_multi_vehicle_.usv1;
            Drone_usv2_ = TVec3(0, -K_multi_usv_formation_distance , m_multi_vehicle_.usv2.current_local_pos.pose.position.z);
            Drone_usv3_ = TVec3(0, -2 * K_multi_usv_formation_distance, m_multi_vehicle_.usv3.current_local_pos.pose.position.z);
            calcFollowUSVPos();

        }
            break;

        case VF_USV_LINE_VERTICAL : {
            is_formation_ = true;
            util_log("usv Formation call! Line Vertical!");
            leader_drone_ = m_multi_vehicle_.usv1;
            Drone_usv2_ = TVec3(K_multi_usv_formation_distance, 0 , m_multi_vehicle_.usv2.current_local_pos.pose.position.z);
            Drone_usv3_ = TVec3(2* K_multi_usv_formation_distance, 0, m_multi_vehicle_.usv3.current_local_pos.pose.position.z);
            calcFollowUSVPos();

        }
            break;

        default:
            break;

    }
}

MultiUSVFormation* MultiUSVFormation::getInstance() {
    if (multi_formation == NULL) {
        multi_formation = new MultiUSVFormation();
    }
    return multi_formation;
}


void MultiUSVFormation::GetData() {
    m_multi_vehicle_ = DataMan::getInstance()->GetData();
}

// follow usv number small to large. follow_us1 and follow_usv1_local_target must be the same drone.
void
MultiUSVFormation::calcFollowUSVPos() {
    // 目标相对位置-当前相对位置+当前在该飞机坐标系下的绝对位置
    follow_usv1_.x() = m_multi_vehicle_.usv1.current_local_pos.pose.position.x + Drone_usv2_.x() + follow_usv1_first_local_.x();
    follow_usv1_.y() = m_multi_vehicle_.usv1.current_local_pos.pose.position.y + Drone_usv2_.y() + follow_usv1_first_local_.y();
    follow_usv1_.z() = leader_drone_.current_local_pos.pose.position.z;
    follow_usv1_keep_local_ = TVec3 (Drone_usv2_.x() + follow_usv1_first_local_.x(), Drone_usv2_.y() + follow_usv1_first_local_.y(), 0);

    follow_usv2_.x() = m_multi_vehicle_.usv1.current_local_pos.pose.position.x + Drone_usv3_.x() + follow_usv2_first_local_.x();
    follow_usv2_.y() = m_multi_vehicle_.usv1.current_local_pos.pose.position.y + Drone_usv3_.y() + follow_usv2_first_local_.y();
    follow_usv2_.z() = leader_drone_.current_local_pos.pose.position.z;
    follow_usv2_keep_local_ = TVec3 (Drone_usv3_.x() + follow_usv2_first_local_.x(), Drone_usv3_.y() + follow_usv2_first_local_.y(), 0);

}

void MultiUSVFormation::OnCheckFormationArrived() {
    mavros_msgs::CommandBool arm_cmd;
    if (is_formation_) {
        usv1_reached_ = true;
        arm_cmd.request.value = false;
        DataMan::getInstance()->SetUSVState(arm_cmd, m_multi_vehicle_.usv1.drone_id);
    }

    if (pos_reached(m_multi_vehicle_.usv2.current_local_pos, follow_usv1_, usv_position_allow_reached_)) {
        usv2_reached_ = true;
        arm_cmd.request.value = false;
        DataMan::getInstance()->SetUSVState(arm_cmd, m_multi_vehicle_.usv2.drone_id);
        util_log("usv6 disarm at one point");
    }
    if (pos_reached(m_multi_vehicle_.usv3.current_local_pos, follow_usv2_, usv_position_allow_reached_)) {
        usv3_reached_ = true;
        arm_cmd.request.value = false;
        DataMan::getInstance()->SetUSVState(arm_cmd, m_multi_vehicle_.usv3.drone_id);
        util_log("usv7 disarm at one point");
    }

    if (usv1_reached_ && usv2_reached_ && usv3_reached_) {
        is_formation_ = false;

        usv1_reached_ = false;
        usv2_reached_ = false;
        usv3_reached_ = false;
    }
}

bool
MultiUSVFormation::pos_reached(geometry_msgs::PoseStamped &current_pos, TVec3 &follow_usv_target, float err_allow) {
    float err_px = current_pos.pose.position.x - follow_usv_target.x();
    float err_py = current_pos.pose.position.y - follow_usv_target.y();

    return sqrt(err_px * err_px + err_py * err_py ) < err_allow;
}

void MultiUSVFormation::GetTakeoffPos() {
    if (m_multi_vehicle_.leader_usv.current_state.armed && !is_get_takeoff_pos_) {

        usv1_takeoff_gps_pos_ = GlobalPosition{m_multi_vehicle_.usv1.latitude, m_multi_vehicle_.usv1.longtitude,0};
        usv2_takeoff_gps_pos_ = GlobalPosition{m_multi_vehicle_.usv2.latitude, m_multi_vehicle_.usv2.longtitude,0};
        usv3_takeoff_gps_pos_ = GlobalPosition{m_multi_vehicle_.usv3.latitude, m_multi_vehicle_.usv3.longtitude,0};
        util_log("usv1_takeoff_gps_pos_ = ( %.9f, %.9f)", usv1_takeoff_gps_pos_.latitude, usv1_takeoff_gps_pos_.longitude);
        util_log("usv2_takeoff_gps_pos_ = ( %.9f, %.9f)", usv2_takeoff_gps_pos_.latitude, usv2_takeoff_gps_pos_.longitude);
        util_log("usv3_takeoff_gps_pos_ = ( %.9f, %.9f)", usv3_takeoff_gps_pos_.latitude, usv3_takeoff_gps_pos_.longitude);

        Calculate::getInstance()->GetLocalPos(usv1_takeoff_gps_pos_, usv2_takeoff_gps_pos_, follow_usv1_first_local_);
        Calculate::getInstance()->GetLocalPos(usv1_takeoff_gps_pos_, usv3_takeoff_gps_pos_, follow_usv2_first_local_);

        is_get_takeoff_pos_ = true;
    }
}

void MultiUSVFormation::DoProgress() {
    GetTakeoffPos();
    OnCheckFormationArrived();
    SetFunctionOutPut();
}

geometry_msgs::PoseStamped MultiUSVFormation::CalculateTargetPos(geometry_msgs::PoseStamped& target_local_pos, Eigen::Matrix<float, 3, 1> formation_target) {
    geometry_msgs::PoseStamped target_local_pos_sp;
    util_log("formation_target (%.2f, %.2f, %.2f)", formation_target(0), formation_target(1), formation_target(2));
    target_local_pos_sp.pose.position.x = target_local_pos.pose.position.x + formation_target(0);
    target_local_pos_sp.pose.position.y = target_local_pos.pose.position.y + formation_target(1);
    target_local_pos_sp.pose.position.z = target_local_pos.pose.position.z;
    return target_local_pos_sp;
}

void MultiUSVFormation::SetFunctionOutPut() {
    leader_usv_id_ = m_multi_vehicle_.leader_usv.drone_id;
    m_multi_vehicle_.usv1.follower_to_leader_pos = TVec3{0, 0, 0};
    m_multi_vehicle_.usv2.follower_to_leader_pos = follow_usv1_;
    m_multi_vehicle_.usv3.follower_to_leader_pos = follow_usv2_;

    m_multi_vehicle_.usv1.follower_keep_pos = TVec3(0, 0, 0);
    m_multi_vehicle_.usv2.follower_keep_pos = follow_usv1_keep_local_;
    m_multi_vehicle_.usv3.follower_keep_pos = follow_usv2_keep_local_;
    DataMan::getInstance()->SetUSVFormationData(m_multi_vehicle_, is_formation_);

    if (is_formation_) {
        util_log("formation boat output!!!!!");
        geometry_msgs::PoseStamped leader_curr{};
        leader_curr.pose.position.z = m_multi_vehicle_.leader_usv.current_local_pos.pose.position.z;

        m_multi_vehicle_.usv1.target_local_pos_sp = CalculateTargetPos(leader_curr , leader_curr_pos_) ;
        m_multi_vehicle_.usv2.target_local_pos_sp = CalculateTargetPos(leader_curr , follow_usv1_) ;
        m_multi_vehicle_.usv3.target_local_pos_sp = CalculateTargetPos(leader_curr , follow_usv2_) ;
        m_multi_vehicle_.leader_usv.target_local_pos_sp = m_multi_vehicle_.usv1.target_local_pos_sp;
        DataMan::getInstance()->SetBoatControlData(m_multi_vehicle_);

    }
}