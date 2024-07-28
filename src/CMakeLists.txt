configure_file(${CMAKE_CURRENT_SOURCE_DIR}/generate_macros.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/generate_macros.hpp)

# add_executable(${PROJECT_NAME}
#    main.cpp
#    program_options.cpp
#    app.cpp
#    app_flags.cpp
#    app_cli_commands.cpp
#    package.cpp
#    file.cpp
#    package_manager.cpp
#    output.cpp
#    string.cpp)
add_executable(${PROJECT_NAME}
  main.cpp
  program_options.cpp
  app.cpp
  app_flags.cpp
  app_cli_commands.cpp
  package.cpp
  file.cpp
  thread_pool.cpp
  package_manager.cpp
  output.cpp)
  
target_include_directories(${PROJECT_NAME}
  PRIVATE
  ${CMAKE_CURRENT_BINARY_DIR})

set(PROJECT_DEFINITIONS)

if(UNIX)
  list(APPEND PROJECT_DEFINITIONS -DAPP_UNIX)
elseif(WIN32)
  list(APPEND PROJECT_DEFINITIONS -DAPP_WINDOWS)
endif()
target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_DEFINITIONS})

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)