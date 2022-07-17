package main

import (
	"encoding/json"
	"errors"
	"fmt"
	"io"
	"io/ioutil"
	"net/http"
	"os"
	"os/exec"
	"path"
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
	e.GET("/server/log", showLog)
	e.GET("/servers", listServers)
	e.GET("/servers/running", listRunningServers)

	// PUTs
	e.PUT("/server/start", startServer)
	e.PUT("/server/stop", stopServer)

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

	id, err := strconv.ParseInt(idStr, 10, 64)
	if err != nil {
		return c.String(http.StatusBadRequest, "Unable to read ID: "+err.Error())
	}

	temp, err := getSingleServerData(c.Request().Context(), id)

	server := convertDBServerToResponseServer(temp)

	// Might lead to hard to find bugs
	if temp == nil || err != nil {
		return c.String(http.StatusNotFound, "Server does not exist!")
	}

	return c.JSON(http.StatusOK, server)
}

func showLog(c echo.Context) error {
	idStr := c.QueryParam("id")

	if len(idStr) < 1 {
		return c.String(http.StatusBadRequest, "Url Param 'id' is missing")
	}

	id, err := strconv.ParseInt(idStr, 10, 64)
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
	servers, err := collectServerData(c.Request().Context())
	if err != nil {
		return c.String(http.StatusInternalServerError, "Unable to retrieve servers: "+err.Error())
	}
	if servers == nil {
		return c.JSON(http.StatusOK, []responseServer{})
	}

	return c.JSON(http.StatusOK, servers)
}

func listRunningServers(c echo.Context) error {
	if runningServers.servers.Len() == 0 {
		return c.String(http.StatusOK, "[]")
	}

	var temp []int64

	for e := runningServers.servers.Front(); e != nil; e = e.Next() {
		temp = append(temp, e.Value.(runningServer).ID)
	}

	return c.JSON(http.StatusOK, temp)
}

// PUTs

// TODO: Break actual server starting code to separate function
// TODO: Add writing output of exec to file to catch output of server that fails before it starts writing to the log
// TODO: Add way of checking if server is still alive and produce errors, warnings, and other output, and remove it from the list of running servers if it exits outside stopServer()
func startServer(c echo.Context) error {
	idStr := c.QueryParam("id")

	if len(idStr) < 1 {
		return c.String(http.StatusBadRequest, "Url Param 'id' is missing")
	}

	id, err := strconv.ParseInt(idStr, 10, 64)
	if err != nil {
		return c.String(http.StatusBadRequest, "Unable to read ID: "+err.Error())
	}

	serverInfo, err := getSingleServerData(c.Request().Context(), id)
	if err != nil {
		return c.String(http.StatusInternalServerError, "Unable to fetch server data: "+err.Error())
	}
	if serverInfo == nil {
		return c.String(http.StatusNotFound, "Server does not exist!")
	}

	if _, err := os.Stat(path.Join(serverInfo.Directory, serverInfo.JarFile)); errors.Is(err, os.ErrNotExist) {
		return c.String(http.StatusBadRequest, "Server jar file does not exist!")
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

	runningServers.servers.PushBack(runningServer{id, stdin, log})

	go log.readLog()

	return c.NoContent(http.StatusOK)
}

// TODO: Wait until child process has exited before removing it from running servers
func stopServer(c echo.Context) error {
	idStr := c.QueryParam("id")

	if len(idStr) < 1 {
		return c.String(http.StatusBadRequest, "Url Param 'id' is missing")
	}

	id, err := strconv.ParseInt(idStr, 10, 64)
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

// PATCHs

func modifyServer(c echo.Context) error {
	idStr := c.QueryParam("id")

	if len(idStr) < 1 {
		return c.String(http.StatusBadRequest, "Url Param 'id' is missing")
	}

	id, err := strconv.ParseInt(idStr, 10, 64)
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

	temp, err := getSingleServerData(c.Request().Context(), id)
	if err != nil {
		return c.String(http.StatusInternalServerError, "Unable to fetch server data: "+err.Error())
	}
	if temp == nil {
		return c.String(http.StatusNotFound, "Server does not exist!")
	}

	// TODO: Add ability to ignore a specific server (for updating a server without changing the name/directory)
	duplicate, err := getServerByDirectory(c.Request().Context(), server.Directory)
	if err != nil {
		return c.String(http.StatusInternalServerError, "Failed checking if server is a duplicate: "+err.Error())
	}

	if duplicate != nil && duplicate.ID != id {
		return c.String(http.StatusBadRequest, "Server with that name or folder already exists.")
	}

	err = modifyServerEntry(c.Request().Context(), server, id)
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

	if _, err := os.Stat(server.Directory); errors.Is(err, os.ErrNotExist) {
		return c.String(http.StatusBadRequest, "Server directory does not exist!")
	}

	duplicate, err := getServerByDirectory(c.Request().Context(), server.Directory)
	if err != nil {
		return c.String(http.StatusInternalServerError, "Unable to check if the given server exists: "+err.Error())
	}

	if duplicate != nil {
		return c.String(http.StatusBadRequest, "Server already exists!")
	}

	err = addServerToDatabase(c.Request().Context(), server)
	if err != nil {
		return c.String(http.StatusInternalServerError, "Failed to add server to database: "+err.Error())
	}

	dbServer, err := getServerByDirectory(c.Request().Context(), server.Directory)
	if err != nil {
		return c.String(http.StatusInternalServerError, "Failed to read new server: "+err.Error())
	}

	// TODO: Create server dir
	// TODO: Write eula file to server dir

	return c.String(http.StatusOK, fmt.Sprintf("%d", dbServer.ID))
}

// DELETEs

func removeServer(c echo.Context) error {
	idStr := c.QueryParam("id")

	if len(idStr) < 1 {
		return c.String(http.StatusBadRequest, "Url Param 'id' is missing")
	}

	id, err := strconv.ParseInt(idStr, 10, 64)
	if err != nil {
		return c.String(http.StatusBadRequest, "Unable to read ID: "+err.Error())
	}

	temp, err := getSingleServerData(c.Request().Context(), id)

	// Might lead to hard to find bugs
	if temp == nil || err != nil {
		return c.String(http.StatusNotFound, "Server does not exist!")
	}

	temp2 := runningServers.Find(id)

	if temp2 != nil {
		serverInfo := temp2.Value.(runningServer)

		stdin := serverInfo.Stdin
		go func() {
			defer stdin.Close()
			io.WriteString(stdin, "stop\n")
		}()

		runningServers.servers.Remove(temp2)
	}

	removeServerFromDB(c.Request().Context(), id)

	return c.String(http.StatusOK, "Server removed")
}

// Helpers

func convertDBServerToResponseServer(dbServer *databaseServer) *responseServer {
	return &responseServer{
		ID:          dbServer.ID,
		Name:        dbServer.Name,
		Directory:   dbServer.Directory,
		StartMemory: dbServer.StartMemory,
		RunMemory:   dbServer.RunMemory,
		JarFile:     dbServer.JarFile,
		JavaArgs:    dbServer.JavaArgs,
		Status:      getServerStatus(dbServer.ID),
	}
}
