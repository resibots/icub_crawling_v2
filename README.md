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



# Notes
- tutorial to control the motors: http://wiki.icub.org/brain/icub_motor_control_tutorial.html