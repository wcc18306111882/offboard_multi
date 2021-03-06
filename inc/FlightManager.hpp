//
// Created by zhouhua on 19-9-22.
//

#ifndef OFFBOARD_FLIGHTMANAGER_HPP
#define OFFBOARD_FLIGHTMANAGER_HPP

#include "Cinc.hpp"

#include "IMsgRosManager.hpp"
#include "IVehicleControl.hpp"
#include "IControlFunction.hpp"
#include "DataMan.hpp"
//#include "PathCreator.hpp"
#include <stack>

class FlightManager : public IFlightDataCallback{
public:

    FlightManager();
    ~FlightManager() {};

    void OnInitConfig(IMsgRosManager* msg_manager);

    void DoProgress();

    void GetData();

    static FlightManager* getInstance();

    void AddVehicleControlProgress(IVehicleControlFactory *IVehicleControl);

    void AddFunctionProgress(IFunctionFactory *factory);

    void FunctionStateUpdate(IFunctionFactory *factory);

    void OnFlightDataUpdate(FDATA_TYPE data_type) override ;

private:

    static FlightManager* l_pInst;
    int factory_size_ = 0;

    bool drone_avodiance_update_ = false;

    IMsgRosManager* msg_manager_;

    list<IVehicleControl*>  m_vehicle_control_list_{};
    vector<IControlFunction*> m_control_function_vector_;

    IControlFunction* m_func_;
    IVehicleControl* cur_f;
    multi_vehicle m_multi_vehicle_;
};

#endif //OFFBOARD_FLIGHTMANAGER_HPP
