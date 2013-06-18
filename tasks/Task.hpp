/* Generated from orogen/lib/orogen/templates/tasks/Task.hpp */

#ifndef CAMERA_ARAVIS_TASK_TASK_HPP
#define CAMERA_ARAVIS_TASK_TASK_HPP

#include "camera_aravis/TaskBase.hpp"
#include <arv.h>


namespace camera_aravis {

    /*! \class Task 
     * \brief The task context provides and requires services. It uses an ExecutionEngine to perform its functions.
     * Essential interfaces are operations, data flow ports and properties. These interfaces have been defined using the oroGen specification.
     * In order to modify the interfaces you should (re)use oroGen and rely on the associated workflow.
     * 
     * \details
     * The name of a TaskContext is primarily defined via:
     \verbatim
     deployment 'deployment_name'
         task('custom_task_name','camera_aravis::Task')
     end
     \endverbatim
     *  It can be dynamically adapted when the deployment is called with a prefix argument. 
     */
    class Task : public TaskBase
    {
	friend class TaskBase;
    public:
        Task(std::string const& name = "camera_aravis::Task");
        Task(std::string const& name, RTT::ExecutionEngine* engine);
	~Task();

        bool configureHook();

    };
    void triggerFunction(const void *p);
}

#endif

