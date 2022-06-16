package main

import (
	"fmt"

	"github.com/spf13/viper"
)

type configuration struct {
	Daemon    daemonConfig
	Database  databaseConfig
	FileStore fileStoreConfig
}

type daemonConfig struct {
	Verbose   bool
	LogFile   string
	JavaPath  string
	Port      int
	ServerDir string
}

type databaseConfig struct {
	Database         string
	DatabaseHost     string
	DatabasePort     int
	DatabaseDatabase string
	DatabaseUser     string
	DatabasePassword string
}

type fileStoreConfig struct {
	JarStore    string
	ModStore    string
	PluginStore string
}

var config configuration

func main() {
	// servers = newServerList()
	runningServers = newRunningServerList()

	viper.SetConfigName("minecraft-daemon")
	viper.SetConfigType("toml")
	viper.AddConfigPath(".")
	err := viper.ReadInConfig()
	if err != nil {
		panic(fmt.Errorf("fatal error in config file: %s", err))
	}
	viper.Unmarshal(&config)

	// This should be done by an installer or initial script.
	// databaseSetup()

	connectDatabase()

	fmt.Println("Ready for requests")
	handleRequests()
}
