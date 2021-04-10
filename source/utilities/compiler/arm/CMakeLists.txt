# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    retarget_io.c
)

if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/CMSIS/CMSIS)
  target_include_directories(${VSF_LIB_NAME} INTERFACE
    3rd-party/CMSIS/CMSIS/Core/Include
    3rd-party/CMSIS/CMSIS/DSP/Include
    3rd-party/CMSIS/CMSIS/NN/Include
  )
endif()
