/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "Task.hpp"
#include <arv.h>
#include <base/logging.h>

using namespace camera_aravis;

void arv_viewer_new_buffer_cb (ArvStream *stream, camera_aravis::Task *task);

Task::Task(std::string const& name)
    : TaskBase(name)
{
    g_type_init();
    printf("Creating interface");
    arv_enable_interface ("Fake");
    printf("... ok\n");
}

Task::Task(std::string const& name, RTT::ExecutionEngine* engine)
    : TaskBase(name, engine)
{
    g_type_init();
    printf("Creating interface");
    arv_enable_interface ("Fake");
    printf("... ok\n");
}

Task::~Task()
{
}

::std::string Task::listCameras(){
	arv_update_device_list ();
	unsigned int n_devices = arv_get_n_devices();
        printf("Found %i Cameras\n",n_devices);
	for (unsigned int i = 0; i < n_devices; i++) {
            printf("Found Camera: %s \n",arv_get_device_id(i));
            camera = arv_camera_new (arv_get_device_id(i));
            printf("Vendor: %s Model: %s\n",arv_camera_get_vendor_name(camera),arv_camera_get_model_name(camera));
            g_object_unref(camera);
        }
        std::string res;
        return res;

}
        


bool Task::configureHook()
{
    //Do not call the parent, because the parent tries to use the camera interface what does not scale for this driver
    //if (! TaskBase::configureHook())
    //    return false;
    newFrame = false; 
    std::string sid = _camera_id.get();
    
    camera = arv_camera_new((sid.c_str()));
    if(camera == 0){
        LOG_FATAL("Could not open Camera with id: %s\n",sid.c_str());
        return false;
    }
    stream = arv_camera_create_stream (camera, NULL, NULL);
    if (stream == 0){
            g_object_unref (camera);
            camera = 0;
            LOG_FATAL("Could not create Stream\n");
            return false;
    }
    last_buffer = 0;
    new_frame_id = 0;
    
    unsigned int frame_rate,payload;
    int width,height;
    bool auto_gain, auto_exposure,is_frame_rate_available,is_exposure_available,is_exposure_auto_available,is_gain_available,is_gain_auto_available;
    double exposure_min, exposure_max,gain_min,gain_max;
    
    unsigned int pixel_format_count;
    gint64 *formats = arv_camera_get_available_pixel_formats(camera,&pixel_format_count);
    for(int i=0;i<pixel_format_count;i++){
        printf("Format: 0x%08x\n",formats[i]);
    }
   
    arv_camera_set_pixel_format(camera,ARV_PIXEL_FORMAT_BAYER_GB_8);

    ArvPixelFormat format = arv_camera_get_pixel_format(camera);
    
    
    arv_stream_set_emit_signals (stream, TRUE);    

    arv_camera_set_region(camera,_region_x.get(),_region_y.get(),_width.get(),_height.get());
    
    payload = arv_camera_get_payload (camera);
    arv_camera_get_region (camera, NULL, NULL, &width, &height);
    ArvPixelFormat pixel_format = arv_camera_get_pixel_format (camera);
    const char *pixel_format_string = arv_camera_get_pixel_format_as_string (camera);
    arv_camera_get_exposure_time_bounds (camera, &exposure_min, &exposure_max);
    arv_camera_get_gain_bounds (camera, &gain_min, &gain_max);
    frame_rate = arv_camera_get_frame_rate (camera);
    auto_gain = arv_camera_get_gain_auto (camera) != ARV_AUTO_OFF;
    auto_exposure = arv_camera_get_exposure_time_auto (camera) != ARV_AUTO_OFF;

    is_frame_rate_available = arv_camera_is_frame_rate_available (camera);
    is_exposure_available = arv_camera_is_exposure_time_available (camera);
    is_exposure_auto_available = arv_camera_is_exposure_auto_available (camera);
    is_gain_available = arv_camera_is_gain_available (camera);
    is_gain_auto_available = arv_camera_is_gain_auto_available (camera);
    LOG_DEBUG("Payload size: %i\n",payload);
    
    printf("Size: %i,%i %i\n",width,height,payload);
    
    for (unsigned i = 0; i < 50; ++i){
        //Preallocating all frames, give the callback function all frames, so prevent memory-copy processes at all,
        //frame id's are recovered in the callback function, update hook will send them out...
        if(format == ARV_PIXEL_FORMAT_BAYER_GB_8){
            //frame[i].init(width,height,8,base::samples::frame::MODE_BAYER_GBRG,128,payload);
            frame[i].init(width,height,8,base::samples::frame::MODE_BAYER_GBRG,128,payload);
        }else{
            LOG_FATAL("Currently no known Pixel format set in Camera, so configuration failed\n");
            return false;
        }
        
        camera_frame[i].reset(&frame[i]);
        frame_ptr[i] = camera_frame[i].write_access();
        camera_frame[i].reset(frame_ptr[i]);
        arv_stream_push_buffer (stream, arv_buffer_new (payload, frame_ptr[i]->getImagePtr()));
    }

    
    _width.set(width);
    _height.set(height);

    LOG_INFO("Gain from %f to %f allowed, Auto Gain: %s\n",gain_min,gain_max,is_gain_auto_available?"YES":"NO");
    LOG_INFO("Gain from %f to %f allowed, Auto Gain: %s\n",gain_min,gain_max,is_gain_auto_available?"YES":"NO");
    LOG_INFO("Exposure from %f to %f allowed, Auto Exposure: %s\n",exposure_min,exposure_max,is_exposure_auto_available?"YES":"NO");
   
    exposure = arv_camera_get_exposure_time(camera);
    gain = arv_camera_get_gain(camera);

    return true;
}
bool Task::startHook()
{
    arv_camera_start_acquisition(camera);
    g_signal_connect (stream, "new-buffer", G_CALLBACK (arv_viewer_new_buffer_cb), this);

    return true;
}

void Task::updateHook()
{
    //Do not call the parent, because the parent tries to use the camera interface what does not scale for this driver
    if(exposure != _exposure.get()){
        LOG_DEBUG("Setting exposure from: %f to :%f\n",exposure,_exposure.get());
        arv_camera_set_exposure_time(camera,_exposure.get());
        exposure = arv_camera_get_exposure_time(camera);
    }
    if(gain != _gain.get()){
        LOG_DEBUG("Setting gain from: %f to :%f\n",gain,_gain.get());
        arv_camera_set_gain(camera,_gain.get());
        exposure = arv_camera_get_gain(camera);
    }
    if(newFrame){
        _frame.write(camera_frame[new_frame_id]);
        newFrame=false;
    }
}

// void Task::errorHook()
// {
//     TaskBase::errorHook();
// }
void Task::stopHook()
{
    //Do not call the parent, because the parent tries to use the camera interface what does not scale for this driver
    //TaskBase::stopHook();
    arv_camera_stop_acquisition(camera);

}
// void Task::cleanupHook()
// {
//     TaskBase::cleanupHook();
// }


void arv_viewer_new_buffer_cb (ArvStream *stream, camera_aravis::Task *task)
{
	ArvBuffer *arv_buffer;

	arv_buffer = arv_stream_pop_buffer (stream);
	if (arv_buffer == NULL)
		return;

	if (arv_buffer->status == ARV_BUFFER_STATUS_SUCCESS) {
                LOG_DEBUG("Got something delta: %f\n",(base::Time::now()-task->lastFrameTime).toSeconds());
                task->lastFrameTime = base::Time::now();
                //Trieing to figure out which frame is the current one to send it in the update hoook,
                //this should normally be the next, but in case os package loss or some unepxected behaviours recover
                task->new_frame_id = (task->new_frame_id+1)%50;
                while(arv_buffer->data != task->frame_ptr[task->new_frame_id]->getImagePtr()){
                    task->new_frame_id = (task->new_frame_id+1)%50;
                    LOG_WARN("Lost Frame sync, this should not happen too much, ideally never\n");
                }
                task->getActivity()->trigger();
                task->newFrame = true;
	}
	if (task->last_buffer != NULL)
		arv_stream_push_buffer (stream, task->last_buffer);
	task->last_buffer = arv_buffer;
}
    
