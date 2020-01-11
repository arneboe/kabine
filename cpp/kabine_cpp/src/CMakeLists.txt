find_package(PkgConfig REQUIRED) 
pkg_check_modules(GP2 REQUIRED libgphoto2)

find_package(Qt5 COMPONENTS Quick Core Gui Widgets REQUIRED)
qt5_add_resources(kabine_cpp_SRCS resources.qrc)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC_FILES 
    StateMachine.cpp
    DigitalCamera.cpp 
    CameraHandler.cpp
    ImageProvider.cpp
    QmlSignalRouter.cpp
    Buttons.cpp)

add_definitions(${Qt5Widgets_DEFINITIONS} ${QtQml_DEFINITIONS} ${${Qt5Quick_DEFINITIONS}})
    
add_executable(kabine_cpp main.cpp ${kabine_cpp_SRCS} ${SRC_FILES})
qt5_use_modules(kabine_cpp Quick Core Gui Widgets )
target_link_libraries(kabine_cpp ${GP2_LIBRARIES} Qt5::Qml Qt5::Gui Qt5::Quick pthread wiringpi)

