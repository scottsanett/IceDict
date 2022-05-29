# Install script for directory: /Users/scott/Downloads/quazip-1.3/quazip

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "RELEASE")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/Users/scott/Downloads/quazip-1.3/quazip/libquazip1-qt6.1.3.dylib"
    "/Users/scott/Downloads/quazip-1.3/quazip/libquazip1-qt6.1.3.0.dylib"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libquazip1-qt6.1.3.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libquazip1-qt6.1.3.0.dylib"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      execute_process(COMMAND /usr/bin/install_name_tool
        -delete_rpath "/opt/homebrew/lib"
        "${file}")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/scott/Downloads/quazip-1.3/quazip/libquazip1-qt6.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libquazip1-qt6.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libquazip1-qt6.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/opt/homebrew/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libquazip1-qt6.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libquazip1-qt6.dylib")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/QuaZip-Qt6-1.3/quazip" TYPE FILE FILES
    "/Users/scott/Downloads/quazip-1.3/quazip/JlCompress.h"
    "/Users/scott/Downloads/quazip-1.3/quazip/ioapi.h"
    "/Users/scott/Downloads/quazip-1.3/quazip/minizip_crypt.h"
    "/Users/scott/Downloads/quazip-1.3/quazip/quaadler32.h"
    "/Users/scott/Downloads/quazip-1.3/quazip/quachecksum32.h"
    "/Users/scott/Downloads/quazip-1.3/quazip/quacrc32.h"
    "/Users/scott/Downloads/quazip-1.3/quazip/quagzipfile.h"
    "/Users/scott/Downloads/quazip-1.3/quazip/quaziodevice.h"
    "/Users/scott/Downloads/quazip-1.3/quazip/quazip.h"
    "/Users/scott/Downloads/quazip-1.3/quazip/quazip_global.h"
    "/Users/scott/Downloads/quazip-1.3/quazip/quazip_qt_compat.h"
    "/Users/scott/Downloads/quazip-1.3/quazip/quazipdir.h"
    "/Users/scott/Downloads/quazip-1.3/quazip/quazipfile.h"
    "/Users/scott/Downloads/quazip-1.3/quazip/quazipfileinfo.h"
    "/Users/scott/Downloads/quazip-1.3/quazip/quazipnewinfo.h"
    "/Users/scott/Downloads/quazip-1.3/quazip/unzip.h"
    "/Users/scott/Downloads/quazip-1.3/quazip/zip.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/QuaZip-Qt6-1.3/QuaZip-Qt6_SharedTargets.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/QuaZip-Qt6-1.3/QuaZip-Qt6_SharedTargets.cmake"
         "/Users/scott/Downloads/quazip-1.3/quazip/CMakeFiles/Export/lib/cmake/QuaZip-Qt6-1.3/QuaZip-Qt6_SharedTargets.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/QuaZip-Qt6-1.3/QuaZip-Qt6_SharedTargets-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/QuaZip-Qt6-1.3/QuaZip-Qt6_SharedTargets.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/QuaZip-Qt6-1.3" TYPE FILE FILES "/Users/scott/Downloads/quazip-1.3/quazip/CMakeFiles/Export/lib/cmake/QuaZip-Qt6-1.3/QuaZip-Qt6_SharedTargets.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/QuaZip-Qt6-1.3" TYPE FILE FILES "/Users/scott/Downloads/quazip-1.3/quazip/CMakeFiles/Export/lib/cmake/QuaZip-Qt6-1.3/QuaZip-Qt6_SharedTargets-release.cmake")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/QuaZip-Qt6-1.3" TYPE FILE FILES
    "/Users/scott/Downloads/quazip-1.3/quazip/QuaZip-Qt6Config.cmake"
    "/Users/scott/Downloads/quazip-1.3/quazip/QuaZip-Qt6ConfigVersion.cmake"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/Users/scott/Downloads/quazip-1.3/quazip/quazip1-qt6.pc")
endif()

