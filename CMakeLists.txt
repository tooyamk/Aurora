cmake_minimum_required (VERSION 3.10.0)

set(CMAKE_DEBUG_POSTFIX d)
#set(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "add a postfix, usually d on windows")
#set(CMAKE_RELEASE_POSTFIX "" CACHE STRING "add a postfix, usually empty on windows")
#set(CMAKE_RELWITHDEBINFO_POSTFIX "rd" CACHE STRING "add a postfix, usually empty on windows")
#set(CMAKE_MINSIZEREL_POSTFIX "s" CACHE STRING "add a postfix, usually empty on windows")

macro(__set_project)
    get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
    string(REPLACE " " "_" ProjectId ${ProjectId})
    project(${ProjectId})

    set(__BUILD_DIR ${PROJECT_BINARY_DIR}/build)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${__BUILD_DIR}$<0:>)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${__BUILD_DIR}$<0:>)
    #set(LIBRARY_OUTPUT_PATH ${__BUILD_DIR}$<0:>)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${__BUILD_DIR}$<0:>)
endmacro()

function(__assign_source_group)
    foreach (_source IN ITEMS ${ARGN})
        if (IS_ABSOLUTE "${_source}")
            file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
        else ()
            set(_source_rel "${_source}")
        endif ()
        get_filename_component(_source_path "${_source_rel}" PATH)
        string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
        source_group("${_source_path_msvc}" FILES "${_source}")
    endforeach ()
endfunction(__assign_source_group)
 
function(__add_library_source_group)
    foreach (_source IN ITEMS ${ARGN})
        __assign_source_group(${_source})
    endforeach ()
    add_library(${ARGV})
endfunction(__add_library_source_group)

set(CMAKE_CXX_STANDARD 17)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(Aurora)

set(__AE_THIRDPARTY_FOLDER_NAME ThirdParty)
set(__AE_MODULES_FOLDER_NAME Modules)
set(__AE_EXTENSIONS_FOLDER_NAME Extensions)

add_subdirectory(Libs/zlib)
add_subdirectory(Libs/libpng)
add_subdirectory(Libs/glew)

add_subdirectory(Core)

if (WIN32)
    add_subdirectory(${__AE_MODULES_FOLDER_NAME}/Graphics/WinD3D11)
endif ()
#add_subdirectory(${__AE_MODULES_FOLDER_NAME}/Graphics/WinGlew)

add_subdirectory(${__AE_EXTENSIONS_FOLDER_NAME}/Files/Images/PNGConverter)