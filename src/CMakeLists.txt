cmake_minimum_required(VERSION 3.20)
project(edgeos-uygulamalari LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Svg)

add_executable(edgeos-setup-wizard edgeos-setup-wizard.cpp)
target_link_libraries(edgeos-setup-wizard PRIVATE Qt6::Widgets Qt6::Svg)

add_executable(edgefile edgefile.cpp)
target_link_libraries(edgefile PRIVATE Qt6::Widgets Qt6::Svg)

install(TARGETS edgeos-setup-wizard edgefile RUNTIME DESTINATION bin)
