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
		log.Println("Url Param 'id' is missing")
		return
	}

	id, err := strconv.Atoi(keys[0])
	if err != nil {
		fmt.Fprintf(w, "Unable to read ID: %s", err)
		return
	}

	temp, err := getServerData(id)

	// Might lead to hard to find bugs
	if temp == nil || err != nil {
		fmt.Fprintf(w, "Server does not exist!")
		w.WriteHeader(http.StatusNotFound)
		return
	}

	json.NewEncoder(w).Encode(temp)
}

func startServer(w http.ResponseWriter, r *http.Request) {
	keys, ok := r.URL.Query()["id"]

	if !ok || len(keys[0]) < 1 {
		log.Println("Url Param 'id' is missing")
		return
	}

	id, err := strconv.Atoi(keys[0])
	if err != nil {
		fmt.Fprintf(w, "Unable to read ID: %s", err)
		return
	}

	serverInfo, err := getServerData(id)

	// Might lead to hard to find bugs
	if serverInfo == nil || err != nil {
		fmt.Fprintf(w, "Server does not exist!")
		w.WriteHeader(http.StatusNotFound)
		return
	}

	var cmd *exec.Cmd

	if serverInfo.JavaArgs.Valid {
		cmd = exec.Command(config.Daemon.JavaPath, "-Xmx"+serverInfo.RunMemory, "-Xms"+serverInfo.StartMemory, serverInfo.JavaArgs.String, "-jar", serverInfo.JarFile, "nogui")
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

func stopServer(w http.ResponseWriter, r *http.Request) {
	keys, ok := r.URL.Query()["id"]

	if !ok || len(keys[0]) < 1 {
		log.Println("Url Param 'id' is missing")
		return
	}

	id, err := strconv.Atoi(keys[0])
	if err != nil {
		fmt.Fprintf(w, "Unable to read ID: %s", err)
		return
	}

	temp := runningServers.Find(id)

	if temp == nil {
		fmt.Fprintf(w, "Server is not running!")
		w.WriteHeader(http.StatusTooEarly)
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
		log.Println("Url Param 'id' is missing")
		return
	}

	id, err := strconv.Atoi(keys[0])
	if err != nil {
		fmt.Fprintf(w, "Unable to read ID: %s", err)
		return
	}

	/* TODO: Add check for running server
	 * 		 If running: follow code bellow
	 * 		 Else: read the latest log file in the server dir
	 */

	temp := runningServers.Find(id)

	if temp == nil {
		fmt.Fprintf(w, "Server is not running!")
		w.WriteHeader(http.StatusTooEarly)
		return
	}
	serverInfo := temp.Value.(runningServer)

	fmt.Fprintf(w, "%s", serverInfo.Log.getLog())
}

func listServers(w http.ResponseWriter, r *http.Request) {
	encoder := json.NewEncoder(w)

	servers := collectServerData()
	if servers == nil {
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

// POSTs

func createServer(w http.ResponseWriter, r *http.Request) {
	reqBody, _ := ioutil.ReadAll(r.Body)
	var server requestServer
	err := json.Unmarshal(reqBody, &server)
	if err != nil {
		fmt.Fprintf(w, "Unable to unmarshal json body: %s", err)
		return
	}

	// TODO: Rewrite for checking the database
	// TODO: Add checks for lengths of fields
	// temp := servers.Find(server.ID)

	// if temp != nil {
	// 	fmt.Fprintf(w, "Server ID already exists!")
	// 	w.WriteHeader(http.StatusBadRequest)
	// 	return
	// }

	addServer2Database(server)

	fmt.Fprintf(w, "Server created")
}

// DELETEs

func removeServer(w http.ResponseWriter, r *http.Request) {
	keys, ok := r.URL.Query()["id"]

	if !ok || len(keys[0]) < 1 {
		log.Println("Url Param 'id' is missing")
		return
	}

	id, err := strconv.Atoi(keys[0])
	if err != nil {
		fmt.Fprintf(w, "Unable to read ID: %s", err)
		return
	}

	temp, err := getServerData(id)

	// Might lead to hard to find bugs
	if temp == nil || err != nil {
		fmt.Fprintf(w, "Server does not exist!")
		w.WriteHeader(http.StatusNotFound)
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
