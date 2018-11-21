/*
#include <rw/common/macros.hpp>
#include <rw/common/Log.hpp>

USE_ROBWORK_NAMESPACE
using namespace robwork;

int main() {
    Log::infoLog() << "The using namespace enables us to call Log directly!\n";
    rw::common::Log::infoLog() << "We can still use the native namespace!\n";
    robwork::Log::infoLog() << "but also the general namespace!\n";
    return 0;
}
*/

#include <iostream>
#include <rw/rw.hpp>
#include <rwlibs/pathplanners/rrt/RRTPlanner.hpp>
#include <rwlibs/pathplanners/rrt/RRTQToQPlanner.hpp>
#include <rwlibs/proximitystrategies/ProximityStrategyFactory.hpp>
#include <rw/kinematics.hpp>
#include <fstream>
#include <math.h>


using namespace std;
using namespace rw::common;
using namespace rw::math;
using namespace rw::kinematics;
using namespace rw::loaders;
using namespace rw::models;
using namespace rw::pathplanning;
using namespace rw::proximity;
using namespace rw::trajectory;
using namespace rwlibs::pathplanners;
using namespace rwlibs::proximitystrategies;



#define MAXTIME 10.




bool checkCollisions(Device::Ptr device, const State &state, const CollisionDetector &detector, const Q &q) {
        State testState;
        CollisionDetector::QueryResult data;
        bool colFrom;

        testState = state;
        device->setQ(q,testState);
        colFrom = detector.inCollision(testState,&data);
        if (colFrom) {
                cerr << "Configuration in collision: " << q << endl;
                cerr << "Colliding frames: " << endl;
                FramePairSet fps = data.collidingFrames;
                for (FramePairSet::iterator it = fps.begin(); it != fps.end(); it++) {
                        cerr << (*it).first->getName() << " " << (*it).second->getName() << endl;
                }
                return false;
        }
        return true;
}

int main() {
    rw::math::Math::seed(); //generates random seed for RRT
    const string wcFile = "/home/kasper/RWworkspace/workcells/PA10WorkCell/PA10/pa10.xml";
    const string deviceName = "PA10";

    WorkCell::Ptr wc = WorkCellLoader::Factory::load(wcFile);
    Device::Ptr device = wc->findDevice(deviceName);
    if (device == NULL) {
            cerr << "Device: " << deviceName << " not found!" << endl;
            return 0;
    }
    State state = wc->getDefaultState();



    return 0;
}

