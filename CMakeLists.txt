cmake_minimum_required(VERSION 3.10)

if(NOT START_YEAR)
  set(START_YEAR 2021)
endif()

if(NOT NUM_YEARS)
  set(NUM_YEARS 5)
endif()

if(NOT PDF_FILENAME)
  set(PDF_FILENAME planner)
endif()

if(NOT COMPRESSED_FILE)
  set(COMPRESSED_FILE ${PDF_FILENAME}_compressed)
endif()

set(EXEC_NAME Planner_PDF)

# set the project name
project(Planner_PDF VERSION 1.0)
configure_file(include/planner_pdf_config.h.in planner_pdf_config.h)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
# add the executable
add_executable(${EXEC_NAME} src/planner_pdf.cpp)

target_link_libraries( Planner_PDF hpdf )
target_include_directories( Planner_PDF PUBLIC
                           "${PROJECT_BINARY_DIR}"
                          )
target_include_directories( Planner_PDF PUBLIC
  "${PROJECT_SOURCE_DIR}/include"
                          )

add_custom_target(
  create
  COMMAND ./${EXEC_NAME}
  ${START_YEAR}
  ${NUM_YEARS}
  ${PDF_FILENAME}.pdf
  DEPENDS ${EXEC_NAME}
  )

add_custom_target(
  compress
  COMMAND gs
  -sDEVICE=pdfwrite
  -dCompatibilityLevel=1.4
  -dPDFSETTINGS=/prepress
  -dDownsampleGrayImages=true
  -dGrayImageResolution=72
  -dNOPAUSE
  -dQUIET
  -dBATCH
  -sOutputFile=${COMPRESSED_FILE}.pdf
  ${PDF_FILENAME}.pdf
  DEPENDS create
  )

add_custom_target(
  clangformat
  COMMAND clang-format
  -style=file
  -i
  ${PROJECT_SOURCE_DIR}/include/*
  ${PROJECT_SOURCE_DIR}/src/*
  )

add_custom_target(
  update_samples
  COMMAND cp
  ${PDF_FILENAME}.pdf
  '../samples/${PDF_FILENAME}_${NUM_YEARS}_year.pdf'
  DEPENDS create
  )

add_custom_target(
  update_compressed_samples
  COMMAND ${CMAKE_COMMAND} -E copy
  ${COMPRESSED_FILE}.pdf
  ../samples/${COMPRESSED_FILE}_${NUM_YEARS}_year.pdf
  DEPENDS compress
  )

unset(PDF_FILENAME)
unset(COMPRESSED_FILE)
unset(NUM_YEARS)
unset(START_YEAR)
