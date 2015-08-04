# icub_crawling_v2
New implementation of crawling for the iCub robot

# Installation
- Clone nn2 from: https://github.com/sferes2/nn2
- export NN2_INCLUDE_DIR=path_to_nn2
    caution: it is important to use absolute paths !
- export EIGEN3_INCLUDE_DIR=path_to_eigen3
    in ubuntu, it is /usr/include/eigen3
- cd modules/cpg/
- mkdir build; cd build
- ccmake ..

# Usage

- start yarpserver
- start the simulator or the robot
- While still in the `build` folder, we can launch the module.

  ~~~
  ./cpg --config_file path/to/config_cpg.ini --robot NameOfTheRobot
  ~~~

# Notes
- tutorial to control the motors: http://wiki.icub.org/brain/icub_motor_control_tutorial.html