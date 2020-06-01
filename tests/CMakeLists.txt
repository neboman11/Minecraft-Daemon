find_program(BASH_PROGRAM bash)

if(BASH_PROGRAM)
    add_test(NAME all-tests COMMAND ${BASH_PROGRAM} run-all-tests.sh ${PROJECT_BINARY_DIR}/src/Minecraft-Daemon WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/tests)
endif(BASH_PROGRAM)

#add_test(NAME exit COMMAND Minecraft-Daemon "${PROJECT_SOURCE_DIR}/minecraft-daemon.conf" "<" "exit.txt")
#add_test(NAME start-stop-server COMMAND Minecraft-Daemon "${PROJECT_SOURCE_DIR}/minecraft-daemon.conf" "<" "start-and-stop-server.txt")
