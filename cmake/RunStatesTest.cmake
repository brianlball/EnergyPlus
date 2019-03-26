  # Set up for convenience
  set(TEST_DIR "tst/reset_states")
  #set( ENV{DDONLY} y)

  # Run Full Year
  execute_process(COMMAND "${CMAKE_COMMAND}" -E copy "${SOURCE_DIR}/testfiles/BriansFiles/${IDF_FILE}" "${BINARY_DIR}/${TEST_DIR}/full/in.idf" )
  execute_process(COMMAND "${CMAKE_COMMAND}" -E copy "${SOURCE_DIR}/testfiles/BriansFiles/${EPW_FILE}" "${BINARY_DIR}/${TEST_DIR}/full/in.epw" )
  execute_process(COMMAND "${CMAKE_COMMAND}" -E copy "${BINARY_DIR}/Products/Energy+.idd" "${BINARY_DIR}/${TEST_DIR}/full/Energy+.idd" )
  execute_process(COMMAND "${CMAKE_COMMAND}" -E copy "${BINARY_DIR}/Products/ReadVarsESO.exe" "${BINARY_DIR}/${TEST_DIR}/full/ReadVarsESO.exe" )

  # Find and execute the test executable, passing the argument of the directory to run in
  if( WIN32 )
    set(ECHO_CMD cmd /C echo.)
  else()
    set(ECHO_CMD "echo")
  endif()
  find_program(TEST_EXE TestResetStates PATHS "${BINARY_DIR}/Products/" "${BINARY_DIR}/Products/Release/" "${BINARY_DIR}/Products/Debug/" )
  message( "Executing TestResetStates from ${TEST_EXE}" )
  message( "Passing run directory as ${TEST_DIR}/full" )
  execute_process(COMMAND ${ECHO_CMD} COMMAND "${TEST_EXE}" "${BINARY_DIR}/${TEST_DIR}/full")
  execute_process(COMMAND ${ECHO_CMD} COMMAND "${BINARY_DIR}/${TEST_DIR}/full/ReadVarsESO.exe" WORKING_DIRECTORY "${BINARY_DIR}/${TEST_DIR}/full")
  
  # Clean up
  #execute_process(COMMAND "${CMAKE_COMMAND}" -E remove "${BINARY_DIR}/${TEST_DIR}/full/Energy+.idd" "${BINARY_DIR}/${TEST_DIR}/full/in.epw")

  # Run First half of Year
  #TODO modify IDF to do that
  execute_process(COMMAND "${CMAKE_COMMAND}" -E copy "${SOURCE_DIR}/testfiles/BriansFiles/${IDF_FILE}" "${BINARY_DIR}/${TEST_DIR}/first/in.idf" )
  execute_process(COMMAND "${CMAKE_COMMAND}" -E copy "${SOURCE_DIR}/testfiles/BriansFiles/${EPW_FILE}" "${BINARY_DIR}/${TEST_DIR}/first/in.epw" )
  execute_process(COMMAND "${CMAKE_COMMAND}" -E copy "${BINARY_DIR}/Products/Energy+.idd" "${BINARY_DIR}/${TEST_DIR}/first/Energy+.idd" )
  execute_process(COMMAND "${CMAKE_COMMAND}" -E copy "${BINARY_DIR}/Products/ReadVarsESO.exe" "${BINARY_DIR}/${TEST_DIR}/first/ReadVarsESO.exe" )
  
  # Find and execute the test executable, passing the argument of the directory to run in
  if( WIN32 )
    set(ECHO_CMD cmd /C echo.)
  else()
    set(ECHO_CMD "echo")
  endif()
  find_program(TEST_EXE TestResetStates PATHS "${BINARY_DIR}/Products/" "${BINARY_DIR}/Products/Release/" "${BINARY_DIR}/Products/Debug/" )
  message( "Executing TestResetStates from ${TEST_EXE}" )
  message( "Passing run directory as ${TEST_DIR}/first" )
  execute_process(COMMAND ${ECHO_CMD} COMMAND "${TEST_EXE}" "${BINARY_DIR}/${TEST_DIR}/first")
  execute_process(COMMAND ${ECHO_CMD} COMMAND "${BINARY_DIR}/${TEST_DIR}/first/ReadVarsESO.exe" WORKING_DIRECTORY "${BINARY_DIR}/${TEST_DIR}/first")
  
  # Clean up
  #execute_process(COMMAND "${CMAKE_COMMAND}" -E remove "${BINARY_DIR}/${TEST_DIR}/full/Energy+.idd" "${BINARY_DIR}/${TEST_DIR}/first/in.epw")


  # Run Last half of Year
  #TODO modify IDF to do that
  execute_process(COMMAND "${CMAKE_COMMAND}" -E copy "${SOURCE_DIR}/testfiles/BriansFiles/${IDF_FILE}" "${BINARY_DIR}/${TEST_DIR}/last/in.idf" )
  execute_process(COMMAND "${CMAKE_COMMAND}" -E copy "${SOURCE_DIR}/testfiles/BriansFiles/${EPW_FILE}" "${BINARY_DIR}/${TEST_DIR}/last/in.epw" )
  execute_process(COMMAND "${CMAKE_COMMAND}" -E copy "${BINARY_DIR}/Products/Energy+.idd" "${BINARY_DIR}/${TEST_DIR}/last/Energy+.idd" )
  execute_process(COMMAND "${CMAKE_COMMAND}" -E copy "${BINARY_DIR}/Products/ReadVarsESO.exe" "${BINARY_DIR}/${TEST_DIR}/last/ReadVarsESO.exe" )
  
  # Find and execute the test executable, passing the argument of the directory to run in
  if( WIN32 )
    set(ECHO_CMD cmd /C echo.)
  else()
    set(ECHO_CMD "echo")
  endif()
  find_program(TEST_EXE TestResetStates PATHS "${BINARY_DIR}/Products/" "${BINARY_DIR}/Products/Release/" "${BINARY_DIR}/Products/Debug/" )
  message( "Executing TestResetStates from ${TEST_EXE}" )
  message( "Passing run directory as ${TEST_DIR}/last" )
  execute_process(COMMAND ${ECHO_CMD} COMMAND "${TEST_EXE}" "${BINARY_DIR}/${TEST_DIR}/last")
  execute_process(COMMAND ${ECHO_CMD} COMMAND "${BINARY_DIR}/${TEST_DIR}/last/ReadVarsESO.exe" WORKING_DIRECTORY "${BINARY_DIR}/${TEST_DIR}/last")

  
  #run python
  message("${CMAKE_COMMAND} -E remove ${BINARY_DIR}/${TEST_DIR}/out1 ${BINARY_DIR}/${TEST_DIR}/out2 ${BINARY_DIR}/${TEST_DIR}/out3 ${BINARY_DIR}/${TEST_DIR}/out4")
  
  execute_process(COMMAND "${CMAKE_COMMAND}" -E remove "${BINARY_DIR}/${TEST_DIR}/out1" "${BINARY_DIR}/${TEST_DIR}/out2" "${BINARY_DIR}/${TEST_DIR}/out3" "${BINARY_DIR}/${TEST_DIR}/out4")
  find_package(PythonInterp 2.7 REQUIRED)
  
  message("${PYTHON_EXECUTABLE} ${BINARY_DIR}/${TEST_DIR}/math_diff.py ${BINARY_DIR}/${TEST_DIR}/last/eplusout.csv ${BINARY_DIR}/${TEST_DIR}/first/eplusout.csv ${BINARY_DIR}/${TEST_DIR}/out1 ${BINARY_DIR}/${TEST_DIR}/out2 ${BINARY_DIR}/${TEST_DIR}/out3 ${BINARY_DIR}/${TEST_DIR}/out4")
	
  execute_process(COMMAND "${PYTHON_EXECUTABLE}" "${BINARY_DIR}/${TEST_DIR}/math_diff.py" "${BINARY_DIR}/${TEST_DIR}/last/eplusout.csv" "${BINARY_DIR}/${TEST_DIR}/first/eplusout.csv" "${BINARY_DIR}/${TEST_DIR}/out1" "${BINARY_DIR}/${TEST_DIR}/out2" "${BINARY_DIR}/${TEST_DIR}/out3" "${BINARY_DIR}/${TEST_DIR}/out4")

  #execute_process(COMMAND \"${PYTHON_EXECUTABLE}\" \"${BINARY_DIR}/${TEST_DIR}/math_diff.py\" \"${BINARY_DIR}/${TEST_DIR}/last/eplusout.csv\" \"${BINARY_DIR}/${TEST_DIR}/first/eplusout.csv\")
  
  # Check the outputs and return appropriately
  file(READ "${BINARY_DIR}/${TEST_DIR}/out4" FILE_CONTENT)
  string(FIND "${FILE_CONTENT}" "All Equal" RESULT)
  if( RESULT EQUAL 0 )
    message("Test Passed")
  else()
    message("Test Failed")
  endif()
