find_package(PkgConfig REQUIRED) 
pkg_check_modules(GP2 REQUIRED libgphoto2)


qt5_add_resources(kabine_cpp_SRCS resources.qrc)

set(SRC_FILES DigitalCamera.cpp)

add_executable(kabine_cpp main.cpp ${kabine_cpp_SRCS} ${SRC_FILES})
target_link_libraries(kabine_cpp ${GP2_LIBRARIES} Qt5::Qml Qt5::Gui)

