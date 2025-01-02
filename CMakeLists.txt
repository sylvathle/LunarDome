cmake_minimum_required(VERSION 2.9 FATAL_ERROR)

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

project(MC-pyCLS)


find_package(Geant4 REQUIRED ui_all vis_all)
#find_package(ROOT REQUIRED COMPONENTS RIO)
#find_package(ROOT COMPONENTS RooStats)
#find_package(ROOT REQUIRED COMPONENTS RIO Net)

include(${Geant4_USE_FILE})
#include(${ROOT_USE_FILE})

include_directories(${ROOT_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/include)

file(GLOB sources ${PROJECT_SOURCE_DIR}/include/*.hh)
file(GLOB headers ${PROJECT_SOURCE_DIR}/src/*.cc)

file(GLOB MACRO_FILES
	"macros/vis.mac"
	)

file(COPY ${MACRO_FILES} DESTINATION ${PROJECT_BINARY_DIR})

file(COPY ${DATA_FILES} DESTINATION ${PROJECT_BINARY_DIR})

#file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/data)

#add_executable(sim_sphere sim_sphere.cc ${sources} ${headers})
#add_executable(sim_flat sim_flat.cc ${sources} ${headers})
add_executable(sim sim.cc ${sources} ${headers})
#target_link_libraries(sim_sphere ${Geant4_LIBRARIES})
#target_link_libraries(sim_flat ${Geant4_LIBRARIES})
target_link_libraries(sim ${Geant4_LIBRARIES})
#target_link_libraries(sim ${ROOT_LIBRARIES})
#target_link_libraries(sim ${Geant4_LIBRARIES} PUBLIC ROOT::RIO ROOT::Net)

add_custom_target(Simulation DEPENDS sim)
