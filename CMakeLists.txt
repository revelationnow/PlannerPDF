cmake_minimum_required(VERSION 3.10)

# set the project name
project(Planner_PDF VERSION 1.0)
configure_file(include/planner_pdf_config.h.in planner_pdf_config.h)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
# add the executable
add_executable(Planner_PDF src/planner_pdf.cpp)

target_link_libraries( Planner_PDF hpdf )
target_include_directories( Planner_PDF PUBLIC
                           "${PROJECT_BINARY_DIR}"
                          )
target_include_directories( Planner_PDF PUBLIC
  "${PROJECT_SOURCE_DIR}/include"
                          )


