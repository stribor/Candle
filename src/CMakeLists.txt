cmake_minimum_required(VERSION 3.13)
project(Candle VERSION 1.1.9)

# set(CMAKE_VERBOSE_MAKEFILE ON)

# In-source builds just create a mess, so we disable it
if("${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
    message(SEND_ERROR "In-source builds are not allowed.")
    return()
endif()

set(CMAKE_CXX_STANDARD 17)

# instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)

# create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)

# As moc files are generated in the binary dir, tell CMake
# to always look for includes there
set(CMAKE_INCLUDE_CURRENT_DIR ON)

option(UPDATE_TRANSLATIONS "Update ts files instead of just compiling them to .qm (only needed when before working on translations)" OFF)
option(ENABLE_ADDRESS_SANITIZER "enable address sanitizer if available (only for Debug builds, mutual exclusive to ENABLE_UB_SANITIZER)" ON)
option(ENABLE_UNDEFINED_SANITIZER "enable undefined behaviour sanitizer if available (only for Debug builds, mutual exclusive to ENABLE_ADDRESS_SANITIZER)" OFF)
option(ENABLE_PEDANTIC_MODE "enable pedantic compiling mode that usually produces a lot of more warnings" ON)
option(USE_STD_CONTAINERS "use std:vector instead of QList and QVector" ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
message(STATUS "Candle Qt Version=${QT_VERSION_MAJOR}")


# Handle sanitizer options
if (ENABLE_ADDRESS_SANITIZER OR ENABLE_UNDEFINED_SANITIZER)
    set(ENABLE_SANITIZER BOOL ON)
else()
    set(ENABLE_SANITIZER BOOL OFF)
endif()
if(ENABLE_SANITIZER)
    if(ENABLE_ADDRESS_SANITIZER AND ENABLE_UNDEFINED_SANITIZER)
        message(FATAL_ERROR "Only one option of ENABLE_ADDRESS_SANITIZER and ENABLE_UB_SANITIZER is allowed")
    endif()

    if(NOT MSVC)
        INCLUDE(CheckCCompilerFlag)

        if (ENABLE_ADDRESS_SANITIZER)
            set(CMAKE_REQUIRED_FLAGS "-Werror -fsanitize=address") # Also needs to be a link flag for test to pass
            check_c_compiler_flag("-fsanitize=address" HAVE_FLAG_SANITIZE_ADDRESS)
            if(HAVE_FLAG_SANITIZE_ADDRESS)
                message(STATUS "Address sanitizer enabled for debug builds")
                set(SANITIZE_FLAG "-fsanitize=address")
            else()
                message(STATUS "Address sanitizer not supported")
            endif()
        endif()

        if (ENABLE_UNDEFINED_SANITIZER)
            set(CMAKE_REQUIRED_FLAGS "-Werror -fsanitize=undefined") # Also needs to be a link flag for test to pass
            check_c_compiler_flag("-fsanitize=undefined" HAVE_FLAG_SANITIZE_UNDEFINED)

            if(HAVE_FLAG_SANITIZE_UNDEFINED)
                message(STATUS "Undefined behaviour sanitizer enabled for debug builds")
                set(SANITIZE_FLAG "-fsanitize=undefined")
            else()
                message(STATUS "Undefined behaviour sanitizer not supported")
            endif()
        endif()
    endif()
endif()


if(QT_VERSION_MAJOR EQUAL  6)
    find_package(Qt6 REQUIRED COMPONENTS Core Widgets OpenGL OpenGLWidgets SerialPort LinguistTools)
else()
    find_package(Qt5 REQUIRED COMPONENTS Core Widgets OpenGL SerialPort LinguistTools )
endif()

#find_package(Qt5 COMPONENTS Core Widgets OpenGL SerialPort LinguistTools REQUIRED)

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
		utils/profile.cpp utils/profile.h)

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
		utils/profile.cpp utils/profile.h)

qt_add_resources(SHADER_RSC
    ${PROJECT_SOURCE_DIR}/shaders.qrc
    ${PROJECT_SOURCE_DIR}/images.qrc
    ${PROJECT_SOURCE_DIR}/fonts.qrc
    )

set (TS_FILES
        translations/candle_ru.ts
        translations/candle_es.ts
        translations/candle_fr.ts
        translations/candle_pt.ts
        )
if (UPDATE_TRANSLATIONS)
    qt_create_translation( TRANSLATIONS ${TS_FILES}
            ${SRC_FILES}
            ${SRC_HEADERS}
            frmabout.ui
            frmmain.ui
            frmsettings.ui
            widgets/sliderbox.ui
            )
else()
    qt_add_translation( TRANSLATIONS ${TS_FILES})
endif()


add_executable(Candle ${SRC_FILES} ${SRC_HEADERS} ${SHADER_RSC} utils/profile.cpp utils/profile.h)

if(APPLE)
    set( MACOSX_BUNDLE_INFO_STRING "${CMAKE_PROJECT_NAME}")
    set( MACOSX_BUNDLE_LONG_VERSION_STRING ${CMAKE_PROJECT_VERSION})
    set( MACOSX_BUNDLE_BUNDLE_NAME "${CMAKE_PROJECT_NAME}")
    set( MACOSX_BUNDLE_SHORT_VERSION_STRING ${CMAKE_PROJECT_VERSION})
    set( MACOSX_BUNDLE_BUNDLE_VERSION ${CMAKE_PROJECT_VERSION})
    set( MACOSX_BUNDLE_COPYRIGHT "Copyright ©2019 https://github.com/Denvi")


#    # use otool to extract minimum os version QtCore is compiled to run at so we can work as low is Qt can
#    execute_process(COMMAND otool -l ${_qt5Core_install_prefix}/lib/QtCore.framework/QtCore
#            COMMAND grep -A2 LC_VERSION_MIN_MACOSX
#            COMMAND awk "END{print $2}"
#            OUTPUT_VARIABLE MINOSX OUTPUT_STRIP_TRAILING_WHITESPACE)
#    #		message(STATUS "Min Mac OS version for Qt is ${MINOSX}")
#    if (MINOSX MATCHES "10\\.([0-9]+)" )
#        message(STATUS "Setting OSX deployment target to ${MINOSX} (based on Qt ${Qt5Widgets_VERSION})")
#        set(CMAKE_OSX_DEPLOYMENT_TARGET ${MINOSX})
#    else()
#        message(STATUS "Could not figure minimum OSX version")
#    endif()

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
    add_osx_resource(Candle images/gcode.icns Resources)
    add_osx_resource(Candle ../LICENSE Resources)

    foreach(_qm ${TRANSLATIONS})
        add_osx_resource(Candle ${_qm}  Resources/translations)
    endforeach()

    # Touch app bundle so finder updates icon etc.
    add_custom_command( TARGET Candle
            POST_BUILD
            COMMAND touch "$<TARGET_FILE_DIR:Candle>/../.."
            COMMENT "Touching Candle")

endif()

target_link_libraries(Candle PRIVATE
    Qt::Core
    Qt::Widgets
    Qt::OpenGL
    Qt::Gui
    Qt::SerialPort
    )
if(QT_VERSION_MAJOR EQUAL  6)
    target_link_libraries(Candle PRIVATE Qt::OpenGLWidgets)
endif()

if(UNIX AND NOT APPLE)
    target_compile_definitions(Candle PRIVATE UNIX )
endif()

target_compile_definitions(Candle PRIVATE APP_VERSION="${CMAKE_PROJECT_VERSION}" )
# enable warnings
target_compile_options(Candle PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
        -Wall>
        $<$<CXX_COMPILER_ID:MSVC>:
        /W4>)

if (ENABLE_SANITIZER AND (HAVE_FLAG_SANITIZE_ADDRESS OR HAVE_FLAG_SANITIZE_UNDEFINED))
    target_compile_options(Candle PRIVATE $<$<CONFIG:Debug>:${SANITIZE_FLAG}>)
    target_link_libraries(Candle PRIVATE $<$<CONFIG:Debug>:${SANITIZE_FLAG}>)

    if(APPLE)
        # Xcode adds this when sanitizer is enabled
        target_compile_definitions(Candle PRIVATE $<$<CONFIG:Debug>:_LIBCPP_HAS_NO_ASAN>)
    endif()
endif()
if (USE_STD_CONTAINERS)
    target_compile_definitions(Candle PRIVATE USE_STD_CONTAINERS)
endif()

if (ENABLE_PEDANTIC_MODE)
    if(NOT MSVC)
        target_compile_options(Candle PRIVATE -Wextra -pedantic)
#        -Wpadded
    endif()
endif()
