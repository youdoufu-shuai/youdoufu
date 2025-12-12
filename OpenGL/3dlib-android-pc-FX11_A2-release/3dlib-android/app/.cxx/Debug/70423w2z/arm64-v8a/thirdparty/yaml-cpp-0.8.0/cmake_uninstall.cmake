if(NOT EXISTS "/Users/slpekamer/Desktop/WorkSpace/Android/3dlib-android-pc-FX11_A2-release-20250108 2/3dlib-android/app/.cxx/Debug/70423w2z/arm64-v8a/install_manifest.txt")
  message(FATAL_ERROR "Cannot find install manifest: /Users/slpekamer/Desktop/WorkSpace/Android/3dlib-android-pc-FX11_A2-release-20250108 2/3dlib-android/app/.cxx/Debug/70423w2z/arm64-v8a/install_manifest.txt")
endif()

file(READ "/Users/slpekamer/Desktop/WorkSpace/Android/3dlib-android-pc-FX11_A2-release-20250108 2/3dlib-android/app/.cxx/Debug/70423w2z/arm64-v8a/install_manifest.txt" files)
string(REGEX REPLACE "\n" ";" files "${files}")
foreach(file ${files})
  message(STATUS "Uninstalling $ENV{DESTDIR}${file}")
  if(IS_SYMLINK "$ENV{DESTDIR}${file}" OR EXISTS "$ENV{DESTDIR}${file}")
    exec_program(
      "/Users/slpekamer/Library/Android/sdk/cmake/3.22.1/bin/cmake" ARGS "-E remove \"$ENV{DESTDIR}${file}\""
      OUTPUT_VARIABLE rm_out
      RETURN_VALUE rm_retval
      )
    if(NOT "${rm_retval}" STREQUAL 0)
      message(FATAL_ERROR "Problem when removing $ENV{DESTDIR}${file}")
    endif()
  else(IS_SYMLINK "$ENV{DESTDIR}${file}" OR EXISTS "$ENV{DESTDIR}${file}")
    message(STATUS "File $ENV{DESTDIR}${file} does not exist.")
  endif()
endforeach()
