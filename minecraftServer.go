package main

import (
	"os/exec"

	"github.com/rs/zerolog/log"
)

func execServer(id int64, serverInfo *databaseServer) {
	var cmd *exec.Cmd

	if len(serverInfo.JavaArgs) > 0 {
		cmd = exec.Command(config.Daemon.JavaPath, "-Xmx"+serverInfo.RunMemory, "-Xms"+serverInfo.StartMemory, serverInfo.JavaArgs, "-jar", serverInfo.JarFile, "nogui")
	} else {
		cmd = exec.Command(config.Daemon.JavaPath, "-Xmx"+serverInfo.RunMemory, "-Xms"+serverInfo.StartMemory, "-jar", serverInfo.JarFile, "nogui")
	}

	cmd.Dir = serverInfo.Directory
	stdin, err := cmd.StdinPipe()
	if err != nil {
		log.Error().Msgf("%s", err)
	}
	stdout, err := cmd.StdoutPipe()
	if err != nil {
		log.Error().Msgf("%s", err)
	}
	if err = cmd.Start(); err != nil {
		log.Error().Msgf("%s", err)
	}

	log := NewServerLog(stdout)

	runningServers.servers.PushBack(runningServer{id, stdin, log, serverInfo.Restart})

	go monitorServer(id, log, serverInfo)
}

func monitorServer(id int64, log *ServerLog, serverInfo *databaseServer) {
	log.readLog()

	temp := runningServers.Find(id)
	if temp == nil {
		return
	}
	runningServers.Remove(id)
	if temp.Value.(runningServer).Restart {
		go execServer(id, serverInfo)
	}
}
