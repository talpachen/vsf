# CMakeLists head

target_include_directories(${VSF_LIB_NAME} INTERFACE
  .
)

add_subdirectory_existed(component)
add_subdirectory_existed(hal)
