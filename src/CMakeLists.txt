configure_file(${CMAKE_CURRENT_SOURCE_DIR}/generate_macros.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/generate_macros.hpp)

add_executable(${PROJECT_NAME}
   main.cpp
   program_options.cpp
   app.cpp
   app_flags.cpp
   app_cli_commands.cpp
   package.cpp
   file.cpp
   package_manager.cpp
   output.cpp
   string.cpp)
target_include_directories(${PROJECT_NAME}
   PRIVATE
   ${CMAKE_SOURCE_DIR}
   ${CMAKE_BINARY_DIR})

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)