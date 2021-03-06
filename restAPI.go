package main

import (
	"encoding/json"
	"fmt"
	"io"
	"io/ioutil"
	"log"
	"net/http"
	"os/exec"
	"strconv"

	"github.com/gorilla/mux"
	"github.com/rs/cors"
)

func handleRequests() {
	myRouter := mux.NewRouter().StrictSlash(true)

	// GETs
	myRouter.HandleFunc("/", homePage)
	myRouter.HandleFunc("/server", showServerInfo).Methods("GET")
	myRouter.HandleFunc("/server/start", startServer).Methods("GET")
	myRouter.HandleFunc("/server/stop", stopServer).Methods("GET")
	myRouter.HandleFunc("/server/log", showLog).Methods("GET")
	myRouter.HandleFunc("/servers", listServers).Methods("GET")
	myRouter.HandleFunc("/servers/running", listRunningServers).Methods("GET")
	myRouter.HandleFunc("/check/duplicate", checkForDuplicateServerRequest).Methods("GET")
	myRouter.HandleFunc("/check/duplicate/name", checkForDuplicateServerNameRequest).Methods("GET")
	myRouter.HandleFunc("/check/duplicate/directory", checkForDuplicateServerDirRequest).Methods("GET")

	// PATCHs
	myRouter.HandleFunc("/server", modifyServer).Methods("PATCH")

	// POSTs
	myRouter.HandleFunc("/server", createServer).Methods("POST")

	// DELETEs
	myRouter.HandleFunc("/server", removeServer).Methods("DELETE")

	handler := cors.Default().Handler(myRouter)

	log.Fatal(http.ListenAndServe(":"+fmt.Sprint(config.Daemon.Port), handler))
}

// GETs

func homePage(w http.ResponseWriter, r *http.Request) {
	fmt.Fprintf(w, "Welcome to the Home Page!")
}

func showServerInfo(w http.ResponseWriter, r *http.Request) {
	keys, ok := r.URL.Query()["id"]

	if !ok || len(keys[0]) < 1 {
		w.WriteHeader(http.StatusBadRequest)
		fmt.Fprintf(w, "Url Param 'id' is missing")
		return
	}

	id, err := strconv.Atoi(keys[0])
	if err != nil {
		w.WriteHeader(http.StatusBadRequest)
		fmt.Fprintf(w, "Unable to read ID: %s", err)
		return
	}

	temp, err := getSingleServerData(id)

	// Might lead to hard to find bugs
	if temp == nil || err != nil {
		w.WriteHeader(http.StatusNotFound)
		fmt.Fprintf(w, "Server does not exist!")
		return
	}

	json.NewEncoder(w).Encode(temp)
}

// TODO: Break actual server starting code to separate function
// TODO: Add writing output of exec to file to catch output of server that fails before it starts writing to the log
// TODO: Add way of checking if server is still alive and produce errors, warnings, and other output, and remove it from the list of running servers if it exits outside stopServer()
func startServer(w http.ResponseWriter, r *http.Request) {
	keys, ok := r.URL.Query()["id"]

	if !ok || len(keys[0]) < 1 {
		w.WriteHeader(http.StatusBadRequest)
		fmt.Fprintf(w, "Url Param 'id' is missing")
		return
	}

	id, err := strconv.Atoi(keys[0])
	if err != nil {
		w.WriteHeader(http.StatusBadRequest)
		fmt.Fprintf(w, "Unable to read ID: %s", err)
		return
	}

	serverInfo, err := getSingleServerData(id)

	// Might lead to hard to find bugs
	if serverInfo == nil || err != nil {
		w.WriteHeader(http.StatusNotFound)
		fmt.Fprintf(w, "Server does not exist!")
		return
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
		log.Fatal(err)
	}
	stdout, err := cmd.StdoutPipe()
	if err != nil {
		log.Fatal(err)
	}
	if err = cmd.Start(); err != nil {
		log.Fatal(err)
	}

	log := NewServerLog(stdout)

	runningServers.servers.PushBack(runningServer{1, stdin, log})

	go log.readLog()
}

// TODO: Wait until child process has exited before removing it from running servers
func stopServer(w http.ResponseWriter, r *http.Request) {
	keys, ok := r.URL.Query()["id"]

	if !ok || len(keys[0]) < 1 {
		w.WriteHeader(http.StatusBadRequest)
		fmt.Fprintf(w, "Url Param 'id' is missing")
		return
	}

	id, err := strconv.Atoi(keys[0])
	if err != nil {
		w.WriteHeader(http.StatusBadRequest)
		fmt.Fprintf(w, "Unable to read ID: %s", err)
		return
	}

	temp := runningServers.Find(id)

	if temp == nil {
		w.WriteHeader(http.StatusBadRequest)
		fmt.Fprintf(w, "Server is not running!")
		return
	}
	serverInfo := temp.Value.(runningServer)

	stdin := serverInfo.Stdin
	go func() {
		defer stdin.Close()
		io.WriteString(stdin, "stop\n")
	}()

	runningServers.servers.Remove(temp)
}

func showLog(w http.ResponseWriter, r *http.Request) {
	keys, ok := r.URL.Query()["id"]

	if !ok || len(keys[0]) < 1 {
		w.WriteHeader(http.StatusBadRequest)
		fmt.Fprintf(w, "Url Param 'id' is missing")
		return
	}

	id, err := strconv.Atoi(keys[0])
	if err != nil {
		w.WriteHeader(http.StatusBadRequest)
		fmt.Fprintf(w, "Unable to read ID: %s", err)
		return
	}

	temp := runningServers.Find(id)

	// TODO: If server is not running (meaning temp == nil), send latest log file contents (should be logs/latest.log)
	if temp == nil {
		w.WriteHeader(http.StatusBadRequest)
		fmt.Fprintf(w, "Server is not running!")
		return
	}
	serverInfo := temp.Value.(runningServer)

	fmt.Fprintf(w, "%s", serverInfo.Log.getLog())
}

func listServers(w http.ResponseWriter, r *http.Request) {
	encoder := json.NewEncoder(w)

	servers := collectServerData()
	if servers == nil {
		w.WriteHeader(http.StatusInternalServerError)
		fmt.Fprintf(w, "Error reading server list")
	}

	encoder.Encode(servers)
}

func listRunningServers(w http.ResponseWriter, r *http.Request) {
	encoder := json.NewEncoder(w)

	if runningServers.servers.Len() == 0 {
		fmt.Fprintf(w, "[]")
		return
	}

	var temp []int

	for e := runningServers.servers.Front(); e != nil; e = e.Next() {
		temp = append(temp, e.Value.(runningServer).ID)
	}

	encoder.Encode(temp)
}

// TODO: Add ability to ignore a specific server (for updating a server without changing the name/directory)
func checkForDuplicateServerRequest(w http.ResponseWriter, r *http.Request) {
	nameKeys, ok := r.URL.Query()["name"]

	if !ok || len(nameKeys[0]) < 1 {
		w.WriteHeader(http.StatusBadRequest)
		fmt.Fprintf(w, "Url Param 'name' is missing")
		return
	}

	dirKeys, ok := r.URL.Query()["directory"]

	if !ok || len(dirKeys[0]) < 1 {
		w.WriteHeader(http.StatusBadRequest)
		fmt.Fprintf(w, "Url Param 'directory' is missing")
		return
	}

	duplicate, err := checkForDuplicateServer(nameKeys[0], dirKeys[0])
	if err != nil {
		w.WriteHeader(http.StatusInternalServerError)
		fmt.Fprintf(w, "Failed checking if server is a duplicate: %s", err)
		return
	}

	if duplicate {
		fmt.Fprintf(w, "true")
		return
	}

	fmt.Fprintf(w, "false")
}

func checkForDuplicateServerNameRequest(w http.ResponseWriter, r *http.Request) {
	nameKeys, ok := r.URL.Query()["name"]

	if !ok || len(nameKeys[0]) < 1 {
		w.WriteHeader(http.StatusBadRequest)
		fmt.Fprintf(w, "Url Param 'name' is missing")
		return
	}

	duplicate, err := checkForDuplicateServerName(nameKeys[0])
	if err != nil {
		w.WriteHeader(http.StatusInternalServerError)
		fmt.Fprintf(w, "Failed checking if server is a duplicate: %s", err)
		return
	}

	if duplicate {
		fmt.Fprintf(w, "true")
		return
	}

	fmt.Fprintf(w, "false")
}

func checkForDuplicateServerDirRequest(w http.ResponseWriter, r *http.Request) {
	nameKeys, ok := r.URL.Query()["directory"]

	if !ok || len(nameKeys[0]) < 1 {
		w.WriteHeader(http.StatusBadRequest)
		fmt.Fprintf(w, "Url Param 'directory' is missing")
		return
	}

	duplicate, err := checkForDuplicateServerDir(nameKeys[0])
	if err != nil {
		w.WriteHeader(http.StatusInternalServerError)
		fmt.Fprintf(w, "Failed checking if server is a duplicate: %s", err)
		return
	}

	if duplicate {
		fmt.Fprintf(w, "true")
		return
	}

	fmt.Fprintf(w, "false")
}

// PATCHs

func modifyServer(w http.ResponseWriter, r *http.Request) {
	keys, ok := r.URL.Query()["id"]

	if !ok || len(keys[0]) < 1 {
		w.WriteHeader(http.StatusBadRequest)
		fmt.Fprintf(w, "Url Param 'id' is missing")
		return
	}

	id, err := strconv.Atoi(keys[0])
	if err != nil {
		w.WriteHeader(http.StatusBadRequest)
		fmt.Fprintf(w, "Unable to read ID: %s", err)
		return
	}

	reqBody, _ := ioutil.ReadAll(r.Body)
	var server requestServer
	err = json.Unmarshal(reqBody, &server)
	if err != nil {
		w.WriteHeader(http.StatusBadRequest)
		fmt.Fprintf(w, "Unable to unmarshal json body: %s", err)
		fmt.Printf(string(reqBody))
		return
	}

	// TODO: Give a better explanation of what field is invalid
	validServer := checkFieldLength(server)
	if !validServer {
		w.WriteHeader(http.StatusBadRequest)
		fmt.Fprintf(w, "Field length too long.")
		return
	}

	// TODO: Add ability to ignore a specific server (for updating a server without changing the name/directory)
	duplicate, err := checkForDuplicateServer(server.Name, server.Directory)
	if err != nil {
		w.WriteHeader(http.StatusInternalServerError)
		fmt.Fprintf(w, "Failed checking if server is a duplicate: %s", err)
		return
	}

	if duplicate {
		w.WriteHeader(http.StatusBadRequest)
		fmt.Fprintf(w, "Server with that name or folder already exists.")
		return
	}

	err = modifyServerEntry(server, id)
	if err != nil {
		fmt.Fprintf(w, "Failed updating server in database: %s", err)
	}
	// TODO: Restart server somewhere
}

// POSTs

func createServer(w http.ResponseWriter, r *http.Request) {
	reqBody, _ := ioutil.ReadAll(r.Body)
	var server requestServer
	err := json.Unmarshal(reqBody, &server)
	if err != nil {
		w.WriteHeader(http.StatusBadRequest)
		fmt.Fprintf(w, "Unable to unmarshal json body: %s", err)
		fmt.Printf(string(reqBody))
		return
	}

	// TODO: Give a better explanation of what field is invalid
	validServer := checkFieldLength(server)
	if !validServer {
		w.WriteHeader(http.StatusBadRequest)
		fmt.Fprintf(w, "Field length too long.")
		return
	}

	duplicate, err := checkForDuplicateServer(server.Name, server.Directory)
	if err != nil {
		w.WriteHeader(http.StatusInternalServerError)
		fmt.Fprintf(w, "Unable to check if the given server exists: %s", err)
		return
	}

	if duplicate {
		w.WriteHeader(http.StatusBadRequest)
		fmt.Fprintf(w, "Server already exists!")
		return
	}

	serverID, err := addServerToDatabase(server)
	if err != nil {
		fmt.Fprintf(w, "Failed to add server to database: %s", err)
	}

	// TODO: Create server dir
	// TODO: Write eula file to server dir

	fmt.Fprintf(w, "%d", serverID)
}

// DELETEs

func removeServer(w http.ResponseWriter, r *http.Request) {
	keys, ok := r.URL.Query()["id"]

	if !ok || len(keys[0]) < 1 {
		w.WriteHeader(http.StatusBadRequest)
		fmt.Fprintf(w, "Url Param 'id' is missing")
		return
	}

	id, err := strconv.Atoi(keys[0])
	if err != nil {
		w.WriteHeader(http.StatusBadRequest)
		fmt.Fprintf(w, "Unable to read ID: %s", err)
		return
	}

	temp, err := getSingleServerData(id)

	// Might lead to hard to find bugs
	if temp == nil || err != nil {
		w.WriteHeader(http.StatusNotFound)
		fmt.Fprintf(w, "Server does not exist!")
		return
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
