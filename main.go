package main

import (
	"fmt"

	"github.com/spf13/viper"
)

type configuration struct {
	Daemon   daemonConfig
	Database databaseConfig
}

type daemonConfig struct {
	Verbose  bool
	LogFile  string
	JavaPath string
	Port     int
}

type databaseConfig struct {
	Database     string
	DatabaseFile string
}

var config configuration

func main() {
	servers = newServerList()
	runningServers = newRunningServerList()

	viper.SetConfigName("minecraft-daemon")
	viper.SetConfigType("toml")
	viper.AddConfigPath(".")
	err := viper.ReadInConfig()
	if err != nil {
		panic(fmt.Errorf("Fatal error in config file: %s", err))
	}
	viper.Unmarshal(&config)

	fmt.Println("Ready for requests")
	handleRequests()
}
