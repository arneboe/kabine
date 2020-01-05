find_package(PkgConfig REQUIRED) 
pkg_check_modules(GP2 REQUIRED libgphoto2)

find_package(Qt5 COMPONENTS Quick Core Gui)
qt5_add_resources(kabine_cpp_SRCS resources.qrc)

set(SRC_FILES 
    DigitalCamera.cpp 
    CameraHandler.cpp
    ImageProvider.cpp
    QmlSignalRouter.cpp)

add_executable(kabine_cpp main.cpp ${kabine_cpp_SRCS} ${SRC_FILES})
qt5_use_modules(kabine_cpp Quick Core Gui )
target_link_libraries(kabine_cpp ${GP2_LIBRARIES} Qt5::Qml Qt5::Gui)

