
IF(CMAKE_HOST_WIN32) #true for win64 also
    SET(PATH_SEP ";")
ELSE()
    SET(PATH_SEP ":")
ENDIF()

IF($ENV{JAVA})
    SET(JAVA "$ENV{JAVA}")
ELSE()
    SET(JAVA java)
ENDIF()

EXECUTE_PROCESS(COMMAND
                    "${JAVA}" -cp "lib/junit-4.6.jar${PATH_SEP}sevenzipjbinding-tests.jar${PATH_SEP}lib/sevenzipjbinding.jar${PATH_SEP}lib/sevenzipjbinding-Platform.jar"
                    "-DSINGLEBUNDLE=${SINGLEBUNDLE}"
		    org.junit.runner.JUnitCore net.sf.sevenzipjbinding.junit.AllTestSuite
                                            WORKING_DIRECTORY .
                                            RESULT_VARIABLE RESULT)
                                     IF(RESULT)
                                         MESSAGE(SEND_ERROR "Error during JUnit Tests")
                                     ENDIF(RESULT)
