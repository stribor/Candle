cmake_minimum_required(VERSION 3.13)
project(Candle VERSION 1.1.9)

# set(CMAKE_VERBOSE_MAKEFILE ON)

# In-source builds just create a mess, so we disable it
if("${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
    message(SEND_ERROR "In-source builds are not allowed.")
    return()
endif()

set(CMAKE_CXX_STANDARD 11)

# instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)

# create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)

# As moc files are generated in the binary dir, tell CMake
# to always look for includes there
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 COMPONENTS Core Widgets OpenGL SerialPort REQUIRED)

include_directories(${PROJECT_SOURCE_DIR})

set (SRC_FILES
        main.cpp
        frmmain.cpp
        frmsettings.cpp
        frmabout.cpp
        drawers/gcodedrawer.cpp
        drawers/heightmapborderdrawer.cpp
        drawers/heightmapgriddrawer.cpp
        drawers/heightmapinterpolationdrawer.cpp
        drawers/origindrawer.cpp
        drawers/shaderdrawable.cpp
        drawers/tooldrawer.cpp
        parser/arcproperties.cpp
        parser/gcodeparser.cpp
        parser/gcodepreprocessorutils.cpp
        parser/gcodeviewparse.cpp
        parser/linesegment.cpp
        parser/pointsegment.cpp
        tables/gcodetablemodel.cpp
        tables/heightmaptablemodel.cpp
        widgets/colorpicker.cpp
        widgets/combobox.cpp
        widgets/groupbox.cpp
        widgets/scrollarea.cpp
        widgets/styledtoolbutton.cpp
        widgets/widget.cpp
        widgets/glwidget.cpp
        widgets/slider.cpp
        widgets/sliderbox.cpp
        drawers/selectiondrawer.cpp
        widgets/comboboxkey.cpp
        )

set(SRC_HEADERS
        frmmain.h
        frmsettings.h
        frmabout.h
        drawers/gcodedrawer.h
        drawers/heightmapborderdrawer.h
        drawers/heightmapgriddrawer.h
        drawers/heightmapinterpolationdrawer.h
        drawers/origindrawer.h
        drawers/shaderdrawable.h
        drawers/tooldrawer.h
        parser/arcproperties.h
        parser/gcodeparser.h
        parser/gcodepreprocessorutils.h
        parser/gcodeviewparse.h
        parser/linesegment.h
        parser/pointsegment.h
        tables/gcodetablemodel.h
        tables/heightmaptablemodel.h
        utils/interpolation.h
        utils/util.h
        widgets/colorpicker.h
        widgets/combobox.h
        widgets/groupbox.h
        widgets/scrollarea.h
        widgets/styledtoolbutton.h
        widgets/widget.h
        widgets/glwidget.h
        widgets/slider.h
        widgets/sliderbox.h
        drawers/selectiondrawer.h
        widgets/comboboxkey.h
        )

qt5_add_resources(SHADER_RSC 
    ${PROJECT_SOURCE_DIR}/shaders.qrc
    ${PROJECT_SOURCE_DIR}/images.qrc
    ${PROJECT_SOURCE_DIR}/fonts.qrc
    )

add_executable(Candle ${SRC_FILES} ${SRC_HEADERS} ${SHADER_RSC})

if(APPLE)
    set( MACOSX_BUNDLE_ICON_FILE candle )
    set( MACOSX_BUNDLE_INFO_STRING "${CMAKE_PROJECT_NAME}")
    set( MACOSX_BUNDLE_LONG_VERSION_STRING ${CMAKE_PROJECT_VERSION})
    set( MACOSX_BUNDLE_BUNDLE_NAME "${CMAKE_PROJECT_NAME}")
    set( MACOSX_BUNDLE_SHORT_VERSION_STRING ${CMAKE_PROJECT_VERSION})
    set( MACOSX_BUNDLE_BUNDLE_VERSION ${CMAKE_PROJECT_VERSION})
    set( MACOSX_BUNDLE_COPYRIGHT "Copyright ©2019 https://github.com/Denvi")


    # use otool to extract minimum os version QtCore is compiled to run at so we can work as low is Qt can
    execute_process(COMMAND otool -l ${_qt5Core_install_prefix}/lib/QtCore.framework/QtCore
            COMMAND grep -A2 LC_VERSION_MIN_MACOSX
            COMMAND awk "END{print $2}"
            OUTPUT_VARIABLE MINOSX OUTPUT_STRIP_TRAILING_WHITESPACE)
    #		message(STATUS "Min Mac OS version for Qt is ${MINOSX}")
    if (MINOSX MATCHES "10\\.([0-9]+)" )
        message(STATUS "Setting OSX deployment target to ${MINOSX} (based on Qt ${Qt5Widgets_VERSION})")
        set(CMAKE_OSX_DEPLOYMENT_TARGET ${MINOSX})
    else()
        message(STATUS "Could not figure minimum OSX version")
    endif()

        set( MACOSX_BUNDLE_GUI_IDENTIFIER "")

    macro(add_osx_resource TARGET ASSET LOCATION)
        set_source_files_properties("${ASSET}" PROPERTIES MACOSX_PACKAGE_LOCATION ${LOCATION} )
        set_source_files_properties("${ASSET}"  PROPERTIES XCODE_EXPLICIT_FILE_TYPE "compiled" )
        target_sources(${TARGET} PRIVATE "${ASSET}")
    endmacro()

    # make it and app bundle
    set_target_properties(Candle PROPERTIES MACOSX_BUNDLE TRUE)
    set_property(TARGET Candle PROPERTY MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/MacOSXBundleInfo.plist.in)

    add_osx_resource(Candle images/candle.icns Resources)
endif()

target_link_libraries(Candle
    Qt5::Core
    Qt5::Widgets
    Qt5::OpenGL
    Qt5::Gui
    Qt5::SerialPort
    )

add_compile_definitions(APP_VERSION="1.1.9")
target_compile_definitions(Candle PRIVATE APP_VERSION="${CMAKE_PROJECT_VERSION}" )
# enable warnings
target_compile_options(Candle PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
        -Wall>
        $<$<CXX_COMPILER_ID:MSVC>:
        /W4>)