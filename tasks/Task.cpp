/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "Task.hpp"
#include "camera_aravis/CameraAravis.hpp"

using namespace camera;
using namespace camera_aravis;

Task::Task(std::string const& name)
    : TaskBase(name)
{
}

Task::Task(std::string const& name, RTT::ExecutionEngine* engine)
    : TaskBase(name, engine)
{
}

Task::~Task()
{
}


bool Task::configureHook()
{
    if (! TaskBase::configureHook())
        return false;

    CameraAravis* camera = new CameraAravis();
    camera->openCamera(_camera_id.value());

    cam_interface = camera;
    cam_interface->setCallbackFcn(triggerFunction,this);
    return true;
}

void camera_aravis::triggerFunction(const void *p)
{
	((RTT::TaskContext*)p)->getActivity()->trigger();
}
