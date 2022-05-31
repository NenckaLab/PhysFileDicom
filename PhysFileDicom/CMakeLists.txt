project(PhysFileToDicom.exe)

set(SOURCE_FILES
    PhysFileDicom.cpp
    Command.cpp
    Command.h
    DicomDir.cpp
    DicomDir.h
)

find_package(DCMTK REQUIRED)
if(APPLE)
    find_package(Boost REQUIRED ALL)
    include_directories(${Boost_INCLUDE_DIRS})
else()
    include_directories(/usr/include/boost)
endif()

include_directories(${DCMTK_INCLUDE_DIRS})

#add_library(${PROJECT_NAME} ${SOURCE_FILES})
add_executable(${PROJECT_NAME} ${SOURCE_FILES})


if(APPLE)
    target_link_libraries(${PROJECT_NAME} ${DCMTK_LIBRARIES})
    target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE ${DCMTK_LIBRARIES})
    target_link_libraries(${PROJECT_NAME} PRIVATE /lib64/libz.so)
    #use the static libraries so destinations don't need the dynamic installed
    target_link_libraries(${PROJECT_NAME} PRIVATE /lib64/libboost_system.a)
    target_link_libraries(${PROJECT_NAME} PRIVATE /lib64/libboost_filesystem.a)
    target_link_libraries(${PROJECT_NAME} PRIVATE /lib64/libboost_program_options.a)
endif()


# Install this example rehearsal code along with this CMakeLists.txt file
install(FILES ${SOURCE_FILES} DESTINATION "PhysFileToDicom.exe")
install(FILES "CMakeLists.txt" DESTINATION "PhysFileToDicom.exe")
