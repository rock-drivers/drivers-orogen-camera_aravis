require 'vizkit'


Orocos::initialize


Orocos.run "camera_aravis::Task"=> "Task" do
    task = Orocos::TaskContext.get("Task")
    task.listCameras
    #task.camera_id = "24934672"
    task.camera_id = "The Imaging Source Europe GmbH-29210317"
#    task.camera_id = "Fake_1"
    task.width = 1280
    task.height = 960
#    task.width = 640
#    task.height = 480
    task.configure
    task.start
    Orocos.log_all_ports
    Vizkit.display task.frame

    Vizkit.exec
end
