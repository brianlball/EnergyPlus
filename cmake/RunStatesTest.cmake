  # Set up for convenience
  set(TEST_DIR "tst/reset_states")
  #set( ENV{DDONLY} y)

  # Run Full Year
      message("SimulationState: ${SimulationState}")
	  set(SimulationState "!comment")
	  message("SimulationState: ${SimulationState}")
	  configure_file("${SOURCE_DIR}/testfiles/BriansFiles/${IDF_FILE}" "${BINARY_DIR}/${TEST_DIR}/full/in.idf")
	  message("SimulationState: ${SimulationState}")
      unset(SimulationState)
	  message("SimulationState: ${SimulationState}")
	  #execute_process(COMMAND "${CMAKE_COMMAND}" -E copy "${SOURCE_DIR}/testfiles/BriansFiles/${IDF_FILE}" "${BINARY_DIR}/${TEST_DIR}/full/in.idf" )
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
      message("SimulationState: ${SimulationState}")
	  #set(SimulationState "SimulationStates,RUNPERIOD 1,save,stop,31,24,1;")
 	  set(SimulationState "SimulationStates,RUNPERIOD 1,save,continue,31,24,1;")
	  message("SimulationState: ${SimulationState}")
	  configure_file("${SOURCE_DIR}/testfiles/BriansFiles/${IDF_FILE}" "${BINARY_DIR}/${TEST_DIR}/first/in.idf")
	  message("SimulationState: ${SimulationState}")
      unset(SimulationState)
	  message("SimulationState: ${SimulationState}")
	  #execute_process(COMMAND "${CMAKE_COMMAND}" -E copy "${SOURCE_DIR}/testfiles/BriansFiles/${IDF_FILE}" "${BINARY_DIR}/${TEST_DIR}/first/in.idf" )
	  execute_process(COMMAND "${CMAKE_COMMAND}" -E copy "${SOURCE_DIR}/testfiles/BriansFiles/${EPW_FILE}" "${BINARY_DIR}/${TEST_DIR}/first/in.epw" )
	  execute_process(COMMAND "${CMAKE_COMMAND}" -E copy "${BINARY_DIR}/Products/Energy+.idd" "${BINARY_DIR}/${TEST_DIR}/first/Energy+.idd" )
	  execute_process(COMMAND "${CMAKE_COMMAND}" -E copy "${BINARY_DIR}/Products/ReadVarsESO.exe" "${BINARY_DIR}/${TEST_DIR}/first/ReadVarsESO.exe" )
	  
	  message( "Executing TestResetStates from ${TEST_EXE}" )
	  message( "Passing run directory as ${TEST_DIR}/first" )
	  execute_process(COMMAND ${ECHO_CMD} COMMAND "${TEST_EXE}" "${BINARY_DIR}/${TEST_DIR}/first")
	  execute_process(COMMAND ${ECHO_CMD} COMMAND "${BINARY_DIR}/${TEST_DIR}/first/ReadVarsESO.exe" WORKING_DIRECTORY "${BINARY_DIR}/${TEST_DIR}/first")

  # Clean up
  #execute_process(COMMAND "${CMAKE_COMMAND}" -E remove "${BINARY_DIR}/${TEST_DIR}/full/Energy+.idd" "${BINARY_DIR}/${TEST_DIR}/first/in.epw")


  # Run Last half of Year
  #TODO modify IDF to do that
  message("SimulationState: ${SimulationState}")
  #SET(SimulationState "SimulationStates,RUNPERIOD 1,load,continue,334,24,4;")
  message("SimulationState: ${SimulationState}")
  set(SimulationState "SimulationStates,RUNPERIOD 1,save,stop,31,24,1;")
  configure_file("${SOURCE_DIR}/testfiles/BriansFiles/${IDF_FILE}" "${BINARY_DIR}/${TEST_DIR}/last/in.idf")
  message("SimulationState: ${SimulationState}")
  unset(SimulationState)
  message("SimulationState: ${SimulationState}")
  #execute_process(COMMAND "${CMAKE_COMMAND}" -E copy "${SOURCE_DIR}/testfiles/BriansFiles/${IDF_FILE}" "${BINARY_DIR}/${TEST_DIR}/last/in.idf" )
  execute_process(COMMAND "${CMAKE_COMMAND}" -E copy "${SOURCE_DIR}/testfiles/BriansFiles/${EPW_FILE}" "${BINARY_DIR}/${TEST_DIR}/last/in.epw" )
  execute_process(COMMAND "${CMAKE_COMMAND}" -E copy "${BINARY_DIR}/Products/Energy+.idd" "${BINARY_DIR}/${TEST_DIR}/last/Energy+.idd" )
  execute_process(COMMAND "${CMAKE_COMMAND}" -E copy "${BINARY_DIR}/Products/ReadVarsESO.exe" "${BINARY_DIR}/${TEST_DIR}/last/ReadVarsESO.exe" )
  

  message( "Executing TestResetStates from ${TEST_EXE}" )
  message( "Passing run directory as ${TEST_DIR}/last" )
  execute_process(COMMAND ${ECHO_CMD} COMMAND "${TEST_EXE}" "${BINARY_DIR}/${TEST_DIR}/last")
  execute_process(COMMAND ${ECHO_CMD} COMMAND "${BINARY_DIR}/${TEST_DIR}/last/ReadVarsESO.exe" WORKING_DIRECTORY "${BINARY_DIR}/${TEST_DIR}/last")

  
  #run python
  message("remove any out1's directories from previous runs")
  
  execute_process(COMMAND "${CMAKE_COMMAND}" -E remove "${BINARY_DIR}/${TEST_DIR}/out1" "${BINARY_DIR}/${TEST_DIR}/out2" "${BINARY_DIR}/${TEST_DIR}/out3" "${BINARY_DIR}/${TEST_DIR}/out4")
  find_package(PythonInterp 2.7 REQUIRED)
  
  message("Trying Python Call")
  set(PYTHON_ARGS ${BINARY_DIR}/${TEST_DIR}/math_diff.py ${BINARY_DIR}\\${TEST_DIR}\\full\\eplusout.csv ${BINARY_DIR}\\${TEST_DIR}\\first\\eplusout.csv ${BINARY_DIR}/${TEST_DIR}/out1 ${BINARY_DIR}/${TEST_DIR}/out2 ${BINARY_DIR}/${TEST_DIR}/out3 ${BINARY_DIR}/${TEST_DIR}/out4)
  message("${PYTHON_EXECUTABLE} ${PYTHON_ARGS}")
  execute_process(COMMAND ${PYTHON_EXECUTABLE} ${PYTHON_ARGS} WORKING_DIRECTORY "${BINARY_DIR}/${TEST_DIR}")
  
  # Check the outputs and return appropriately
  file(READ "${BINARY_DIR}/${TEST_DIR}/out4" FILE_CONTENT)
  message("file content")
  message("${FILE_CONTENT}")
  string(FIND "${FILE_CONTENT}" "All Equal" RESULT)
  string(FIND "${FILE_CONTENT}" "Small Diffs" RESULT2)
  #check if string was found (-1 means it wasnt found)
  #look for All Equal
  if( NOT (RESULT EQUAL -1 ))
    message("Test Passed")
  else()
    #look for Small Diffs
    if (NOT (RESULT2 EQUAL -1 ))
      message("Test Passed")
    else()
      message("Test Failed")
    endif()
  endif()
