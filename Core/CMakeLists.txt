__set_project()
set(__OUTPUT_NAME ae-core)
set_property(GLOBAL PROPERTY PROJECT_AE_CORE_NAME ${PROJECT_NAME})
set_property(GLOBAL PROPERTY PROJECT_AE_CORE_SRC_DIR ${PROJECT_SOURCE_DIR})
set_property(GLOBAL PROPERTY PROJECT_AE_CORE_DLL ${AE_BUILD_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}${__OUTPUT_NAME}${AE_BUILD_TYPE_SUFFIX}${AE_DLL_SUFFIX})

include_directories(src)
add_definitions(${AE_DEFINITIONS} -DAE_CORE_EXPORTS)
file(GLOB SRC_FILES
    "src/*.h" "src/*.cpp"
    "src/*/*.h" "src/*/*.cpp"
    "src/*/*/*.h" "src/*/*/*.cpp"
    "src/*/*/*/*.h" "src/*/*/*/*.cpp")
__add_library_source_group(${PROJECT_NAME} SHARED ${SRC_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${__OUTPUT_NAME})
__install(${PROJECT_NAME})

#set(CMAKE_INSTALL_PREFIX ${PROJECT_BINARY_DIR})
#install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})