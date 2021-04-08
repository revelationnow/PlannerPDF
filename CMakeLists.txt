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

if(NOT Planner_PDF_Start_Day)
  set(Planner_PDF_Start_Day 0)
elseif( ${Planner_PDF_Start_Day} GREATER 6)
  message("Start day parameter exceeds 6, make sure to set it to something less. 0 : Sunday, 1 : Monday, 2 : Tuesday, 3 : Wednesday, 4 : Thursday, 5: Friday, 6 : Saturday")
  set(Planner_PDF_Start_Day 0)
endif()

if(NOT Planner_PDF_VERSION_MAJOR)
  set(Planner_PDF_VERSION_MAJOR 1)
endif()

if(NOT Planner_PDF_VERSION_MINOR)
  set(Planner_PDF_VERSION_MINOR 0)
endif()

if(NOT Planner_PDF_Left_Handed)
  set(Planner_PDF_Left_Handed 0)
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

set(HANDEDNESS_TEXT "right")

if(${Planner_PDF_Left_Handed} MATCHES 1)
  set(HANDEDNESS_TEXT "left")
endif()

set(START_DAY_TEXT "Sunday")
if(${Planner_PDF_Start_Day} MATCHES 1)
  set(START_DAY_TEXT "Monday")
elseif(${Planner_PDF_Start_Day} MATCHES 2)
  set(START_DAY_TEXT "Tueday")
elseif(${Planner_PDF_Start_Day} MATCHES 3)
  set(START_DAY_TEXT "Wednesday")
elseif(${Planner_PDF_Start_Day} MATCHES 4)
  set(START_DAY_TEXT "Thursday")
elseif(${Planner_PDF_Start_Day} MATCHES 5)
  set(START_DAY_TEXT "Friday")
elseif(${Planner_PDF_Start_Day} MATCHES 6)
  set(START_DAY_TEXT "Saturday")
endif()

set(SAMPLE_FILE_TAG "${START_YEAR}_${NUM_YEARS}year_${START_DAY_TEXT}_start_${HANDEDNESS_TEXT}_handed_v${Planner_PDF_VERSION_MAJOR}_${Planner_PDF_VERSION_MINOR}"
  )

add_custom_target(
  update_samples
  COMMAND cp
  ${PDF_FILENAME}.pdf
  '../samples/${PDF_FILENAME}_${SAMPLE_FILE_TAG}.pdf'
  DEPENDS create
  )

add_custom_target(
  update_compressed_samples
  COMMAND ${CMAKE_COMMAND} -E copy
  ${COMPRESSED_FILE}.pdf
  '../samples/${COMPRESSED_FILE}_${SAMPLE_FILE_TAG}.pdf'
  DEPENDS compress
  )

unset(PDF_FILENAME)
unset(COMPRESSED_FILE)
unset(NUM_YEARS)
unset(START_YEAR)
