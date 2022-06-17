package main

import (
	"encoding/json"
	"fmt"
	"io"
	"io/ioutil"
	"net/http"
	"os/exec"
	"strconv"

	"github.com/labstack/echo/v4"
	"github.com/labstack/echo/v4/middleware"
	"github.com/rs/zerolog/log"
)

func handleRequests() {
	e := echo.New()
	e.Use(middleware.CORSWithConfig(middleware.CORSConfig{
		AllowOrigins: []string{"*/*"},
		AllowHeaders: []string{echo.HeaderOrigin, echo.HeaderContentType, echo.HeaderAccept},
	}))
	e.Use(middleware.Logger())
	e.Use(middleware.Recover())

	// GETs
	e.GET("/", homePage)
	e.GET("/server", showServerInfo)
	e.GET("/server/start", startServer)
	e.GET("/server/stop", stopServer)
	e.GET("/server/log", showLog)
	e.GET("/servers", listServers)
	e.GET("/servers/running", listRunningServers)
	e.GET("/check/duplicate", checkForDuplicateServerRequest)
	e.GET("/check/duplicate/name", checkForDuplicateServerNameRequest)
	e.GET("/check/duplicate/directory", checkForDuplicateServerDirRequest)

	// PATCHs
	e.PATCH("/server", modifyServer)

	// POSTs
	e.POST("/server", createServer)

	// DELETEs
	e.DELETE("/server", removeServer)

	e.Logger.Fatal(e.Start(":" + fmt.Sprint(config.Daemon.Port)))
}

// GETs

func homePage(c echo.Context) error {
	return c.String(http.StatusOK, "Welcome to the Home Page!")
}

func showServerInfo(c echo.Context) error {
	idStr := c.QueryParam("id")

	if len(idStr) < 1 {
		return c.String(http.StatusBadRequest, "Url Param 'id' is missing")
	}

	id, err := strconv.Atoi(idStr)
	if err != nil {
		return c.String(http.StatusBadRequest, "Unable to read ID: "+err.Error())
	}

	temp, err := getSingleServerData(id)

	// Might lead to hard to find bugs
	if temp == nil || err != nil {
		return c.String(http.StatusNotFound, "Server does not exist!")
	}

	return c.JSON(http.StatusOK, temp)
}

// TODO: Break actual server starting code to separate function
// TODO: Add writing output of exec to file to catch output of server that fails before it starts writing to the log
// TODO: Add way of checking if server is still alive and produce errors, warnings, and other output, and remove it from the list of running servers if it exits outside stopServer()
func startServer(c echo.Context) error {
	idStr := c.QueryParam("id")

	if len(idStr) < 1 {
		return c.String(http.StatusBadRequest, "Url Param 'id' is missing")
	}

	id, err := strconv.Atoi(idStr)
	if err != nil {
		return c.String(http.StatusBadRequest, "Unable to read ID: "+err.Error())
	}

	serverInfo, err := getSingleServerData(id)

	// Might lead to hard to find bugs
	if serverInfo == nil || err != nil {
		return c.String(http.StatusNotFound, "Server does not exist!")
	}

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

	runningServers.servers.PushBack(runningServer{1, stdin, log})

	go log.readLog()

	return c.NoContent(http.StatusOK)
}

// TODO: Wait until child process has exited before removing it from running servers
func stopServer(c echo.Context) error {
	idStr := c.QueryParam("id")

	if len(idStr) < 1 {
		return c.String(http.StatusBadRequest, "Url Param 'id' is missing")
	}

	id, err := strconv.Atoi(idStr)
	if err != nil {
		return c.String(http.StatusBadRequest, "Unable to read ID: "+err.Error())
	}

	temp := runningServers.Find(id)

	if temp == nil {
		return c.String(http.StatusBadRequest, "Server is not running!")
	}
	serverInfo := temp.Value.(runningServer)

	stdin := serverInfo.Stdin
	go func() {
		defer stdin.Close()
		io.WriteString(stdin, "stop\n")
	}()

	runningServers.servers.Remove(temp)

	return c.NoContent(http.StatusOK)
}

func showLog(c echo.Context) error {
	idStr := c.QueryParam("id")

	if len(idStr) < 1 {
		return c.String(http.StatusBadRequest, "Url Param 'id' is missing")
	}

	id, err := strconv.Atoi(idStr)
	if err != nil {
		return c.String(http.StatusBadRequest, "Unable to read ID: "+err.Error())
	}

	temp := runningServers.Find(id)

	// TODO: If server is not running (meaning temp == nil), send latest log file contents (should be logs/latest.log)
	if temp == nil {
		return c.String(http.StatusBadRequest, "Server is not running!")
	}
	serverInfo := temp.Value.(runningServer)

	return c.String(http.StatusOK, serverInfo.Log.getLog())
}

func listServers(c echo.Context) error {
	servers := collectServerData()
	if servers == nil {
		return c.String(http.StatusInternalServerError, "Error reading server list")
	}

	return c.JSON(http.StatusOK, servers)
}

func listRunningServers(c echo.Context) error {
	if runningServers.servers.Len() == 0 {
		return c.String(http.StatusOK, "[]")
	}

	var temp []int

	for e := runningServers.servers.Front(); e != nil; e = e.Next() {
		temp = append(temp, e.Value.(runningServer).ID)
	}

	return c.JSON(http.StatusOK, temp)
}

// TODO: Add ability to ignore a specific server (for updating a server without changing the name/directory)
func checkForDuplicateServerRequest(c echo.Context) error {
	name := c.QueryParam("name")

	if len(name) < 1 {
		return c.String(http.StatusBadRequest, "Url Param 'name' is missing")
	}

	dir := c.QueryParam("directory")

	if len(dir) < 1 {
		return c.String(http.StatusBadRequest, "Url Param 'directory' is missing")
	}

	duplicate, err := checkForDuplicateServer(name, dir)
	if err != nil {
		return c.String(http.StatusInternalServerError, "Failed checking if server is a duplicate: "+err.Error())
	}

	if duplicate {
		return c.String(http.StatusOK, "true")
	}

	return c.String(http.StatusOK, "false")
}

func checkForDuplicateServerNameRequest(c echo.Context) error {
	name := c.QueryParam("name")

	if len(name) < 1 {
		return c.String(http.StatusBadRequest, "Url Param 'name' is missing")
	}

	duplicate, err := checkForDuplicateServerName(name)
	if err != nil {
		return c.String(http.StatusInternalServerError, "Failed checking if server is a duplicate: "+err.Error())
	}

	if duplicate {
		return c.String(http.StatusOK, "true")
	}

	return c.String(http.StatusOK, "false")
}

func checkForDuplicateServerDirRequest(c echo.Context) error {
	dir := c.QueryParam("directory")

	if len(dir) < 1 {
		return c.String(http.StatusBadRequest, "Url Param 'directory' is missing")
	}

	duplicate, err := checkForDuplicateServerDir(dir)
	if err != nil {
		return c.String(http.StatusInternalServerError, "Failed checking if server is a duplicate: "+err.Error())
	}

	if duplicate {
		return c.String(http.StatusOK, "true")
	}

	return c.String(http.StatusOK, "false")
}

// PATCHs

func modifyServer(c echo.Context) error {
	idStr := c.QueryParam("id")

	if len(idStr) < 1 {
		return c.String(http.StatusBadRequest, "Url Param 'id' is missing")
	}

	id, err := strconv.Atoi(idStr)
	if err != nil {
		return c.String(http.StatusBadRequest, "Unable to read ID: "+err.Error())
	}

	reqBody, _ := ioutil.ReadAll(c.Request().Body)
	var server requestServer
	err = json.Unmarshal(reqBody, &server)
	if err != nil {
		log.Info().Msg(string(reqBody))
		return c.String(http.StatusBadRequest, "Unable to unmarshal json body: "+err.Error())
	}

	// TODO: Give a better explanation of what field is invalid
	validServer := checkFieldLength(server)
	if !validServer {
		return c.String(http.StatusBadRequest, "Field length too long")
	}

	// TODO: Add ability to ignore a specific server (for updating a server without changing the name/directory)
	duplicate, err := checkForDuplicateServer(server.Name, server.Directory)
	if err != nil {
		return c.String(http.StatusInternalServerError, "Failed checking if server is a duplicate: "+err.Error())
	}

	if duplicate {
		return c.String(http.StatusBadRequest, "Server with that name or folder already exists.")
	}

	err = modifyServerEntry(server, id)
	if err != nil {
		return c.String(http.StatusInternalServerError, "Failed updating server in database: "+err.Error())
	}
	// TODO: Restart server somewhere

	return c.String(http.StatusOK, "Server updated")
}

// POSTs

func createServer(c echo.Context) error {
	reqBody, _ := ioutil.ReadAll(c.Request().Body)
	var server requestServer
	err := json.Unmarshal(reqBody, &server)
	if err != nil {
		log.Info().Msg(string(reqBody))
		return c.String(http.StatusBadRequest, "Unable to unmarshal json body: "+err.Error())
	}

	// TODO: Give a better explanation of what field is invalid
	validServer := checkFieldLength(server)
	if !validServer {
		return c.String(http.StatusBadRequest, "Field length too long")
	}

	duplicate, err := checkForDuplicateServer(server.Name, server.Directory)
	if err != nil {
		return c.String(http.StatusInternalServerError, "Unable to check if the given server exists: "+err.Error())
	}

	if duplicate {
		return c.String(http.StatusBadRequest, "Server already exists!")
	}

	serverID, err := addServerToDatabase(server)
	if err != nil {
		return c.String(http.StatusInternalServerError, "Failed to add server to database: "+err.Error())
	}

	// TODO: Create server dir
	// TODO: Write eula file to server dir

	return c.String(http.StatusOK, fmt.Sprintf("%d", serverID))
}

// DELETEs

func removeServer(c echo.Context) error {
	idStr := c.QueryParam("id")

	if len(idStr) < 1 {
		return c.String(http.StatusBadRequest, "Url Param 'id' is missing")
	}

	id, err := strconv.Atoi(idStr)
	if err != nil {
		return c.String(http.StatusBadRequest, "Unable to read ID: "+err.Error())
	}

	temp, err := getSingleServerData(id)

	// Might lead to hard to find bugs
	if temp == nil || err != nil {
		return c.String(http.StatusNotFound, "Server does not exist!")
	}

	temp2 := runningServers.Find(id)

	if temp != nil {
		serverInfo := temp2.Value.(runningServer)

		stdin := serverInfo.Stdin
		go func() {
			defer stdin.Close()
			io.WriteString(stdin, "stop\n")
		}()

		runningServers.servers.Remove(temp2)
	}

	// TODO: Rewrite for database
	// servers.servers.Remove(servers.Find(id))

	return c.String(http.StatusOK, "Server removed")
}

// Helpers

func checkFieldLength(server requestServer) bool {
	if len(server.Name) > maxNameLength {
		return false
	}
	if len(server.Directory) > maxDirectoryLength {
		return false
	}
	if len(server.JarFile) > maxJarFileLength {
		return false
	}
	if len(server.RunMemory) > maxRunMemoryLength {
		return false
	}
	if len(server.StartMemory) > maxStartMemoryLength {
		return false
	}
	if len(server.JavaArgs) > maxJavaArgsLength {
		return false
	}

	return true
}
