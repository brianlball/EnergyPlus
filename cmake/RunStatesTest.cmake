  # Set up for convenience
  set(TEST_DIR "tst/reset_states")
  #set( ENV{DDONLY} y)
  execute_process(COMMAND "${CMAKE_COMMAND}" -E remove_directory "${BINARY_DIR}/${TEST_DIR}/split")
  execute_process(COMMAND "${CMAKE_COMMAND}" -E remove_directory "${BINARY_DIR}/${TEST_DIR}/full")

  # Run Full Year
  #copy over idf and use !comment to replace ${SimulationState} var in idf with dummy comment so it still runs full year in /full dir
  set(SimulationState "!comment")
  configure_file("${SOURCE_DIR}/testfiles/BriansFiles/${IDF_FILE}" "${BINARY_DIR}/${TEST_DIR}/full/in.idf")
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
  #run model and create csv
  message( "Executing TestResetStates from ${TEST_EXE}" )
  message( "Passing run directory as ${TEST_DIR}/full" )
  execute_process(COMMAND ${ECHO_CMD} COMMAND "${TEST_EXE}" "${BINARY_DIR}/${TEST_DIR}/full")
  execute_process(COMMAND ${ECHO_CMD} COMMAND "${BINARY_DIR}/${TEST_DIR}/full/ReadVarsESO.exe" WORKING_DIRECTORY "${BINARY_DIR}/${TEST_DIR}/full")

  # Run First half of Year in /split dir
  set(SimulationState "SimulationStates,RUNPERIOD 1,save,stop,182,24,1;")
  configure_file("${SOURCE_DIR}/testfiles/BriansFiles/${IDF_FILE}" "${BINARY_DIR}/${TEST_DIR}/split/in.idf")
  execute_process(COMMAND "${CMAKE_COMMAND}" -E copy "${SOURCE_DIR}/testfiles/BriansFiles/${EPW_FILE}" "${BINARY_DIR}/${TEST_DIR}/split/in.epw" )
  execute_process(COMMAND "${CMAKE_COMMAND}" -E copy "${BINARY_DIR}/Products/Energy+.idd" "${BINARY_DIR}/${TEST_DIR}/split/Energy+.idd" )
  execute_process(COMMAND "${CMAKE_COMMAND}" -E copy "${BINARY_DIR}/Products/ReadVarsESO.exe" "${BINARY_DIR}/${TEST_DIR}/split/ReadVarsESO.exe" )
  
  message( "Executing TestResetStates from ${TEST_EXE}" )
  message( "Passing run directory as ${TEST_DIR}/split" )
  execute_process(COMMAND ${ECHO_CMD} COMMAND "${TEST_EXE}" "${BINARY_DIR}/${TEST_DIR}/split")
  execute_process(COMMAND ${ECHO_CMD} COMMAND "${BINARY_DIR}/${TEST_DIR}/split/ReadVarsESO.exe" WORKING_DIRECTORY "${BINARY_DIR}/${TEST_DIR}/split")
  #make copy of csv for debugging
  execute_process(COMMAND "${CMAKE_COMMAND}" -E copy "${BINARY_DIR}/${TEST_DIR}/split/eplusout.csv" "${BINARY_DIR}/${TEST_DIR}/split/eplusout_first.csv" )

  # Run Last half of Year
  # remove idf and then copy over new idf with load state functionality
  execute_process(COMMAND "${CMAKE_COMMAND}" -E remove "${BINARY_DIR}/${TEST_DIR}/split/in.idf")

  SET(SimulationState "SimulationStates,RUNPERIOD 1,load,continue,182,24,1;")
  configure_file("${SOURCE_DIR}/testfiles/BriansFiles/${IDF_FILE}" "${BINARY_DIR}/${TEST_DIR}/split/in.idf")
  
  message( "Executing TestResetStates from ${TEST_EXE}" )
  message( "Passing run directory as ${TEST_DIR}/split" )
  execute_process(COMMAND ${ECHO_CMD} COMMAND "${TEST_EXE}" "${BINARY_DIR}/${TEST_DIR}/split")
  execute_process(COMMAND ${ECHO_CMD} COMMAND "${BINARY_DIR}/${TEST_DIR}/split/ReadVarsESO.exe" WORKING_DIRECTORY "${BINARY_DIR}/${TEST_DIR}/split")
  
  # run python
  # cmd is: python mathdiff <input1> <input2> <abs_diff_out> <rel_diff_out> <err_log> <summary_csv>
  # we use out1 out2 out3 out4 as the out files 
  # delete them if they already exist before the run
  message("remove any out1's directories from previous runs")
  execute_process(COMMAND "${CMAKE_COMMAND}" -E remove "${BINARY_DIR}/${TEST_DIR}/out1" "${BINARY_DIR}/${TEST_DIR}/out2" "${BINARY_DIR}/${TEST_DIR}/out3" "${BINARY_DIR}/${TEST_DIR}/out4")
  
  find_package(PythonInterp 2.7 REQUIRED)
  
  message("Trying Python Call")
  set(PYTHON_ARGS ${BINARY_DIR}/${TEST_DIR}/math_diff.py ${BINARY_DIR}\\${TEST_DIR}\\full\\eplusout.csv ${BINARY_DIR}\\${TEST_DIR}\\split\\eplusout.csv ${BINARY_DIR}/${TEST_DIR}/out1 ${BINARY_DIR}/${TEST_DIR}/out2 ${BINARY_DIR}/${TEST_DIR}/out3 ${BINARY_DIR}/${TEST_DIR}/out4)
  message("${PYTHON_EXECUTABLE} ${PYTHON_ARGS}")
  execute_process(COMMAND ${PYTHON_EXECUTABLE} ${PYTHON_ARGS} WORKING_DIRECTORY "${BINARY_DIR}/${TEST_DIR}")
  
  # Check the outputs and return appropriately
  if(EXISTS "${BINARY_DIR}/${TEST_DIR}/out4")
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
	  
  elseif(EXISTS "${BINARY_DIR}/${TEST_DIR}/out3")
    message("CSV's are way off.")  
    message("Test Failed")
  else()
    message("Neither Out3 or Out4 created.  really bad.")
    message("Test Failed")
  endif()  
