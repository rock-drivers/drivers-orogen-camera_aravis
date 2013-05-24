require "orocos" 

Orocos::initialize

Orocos.run "camera_aravis::Task"=> "Task" do
    task = Orocos::TaskContext.get("Task")
    task.camera_id = "The Imaging Source Europe GmbH-29210317"
    task.width = 1280
    task.height = 960
    task.configure
    task.start
    Orocos.log_all_ports
    while true
        sleep 10
    end
end
