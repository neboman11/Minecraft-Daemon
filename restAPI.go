package main

import (
	"encoding/json"
	"fmt"
	"io"
	"io/ioutil"
	"log"
	"net/http"
	"os/exec"

	"github.com/gorilla/mux"
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

	log.Fatal(http.ListenAndServe(":"+fmt.Sprint(config.Daemon.Port), myRouter))
}

func homePage(w http.ResponseWriter, r *http.Request) {
	fmt.Fprintf(w, "Welcome to the Home Page!")
}

func showServerInfo(w http.ResponseWriter, r *http.Request) {
	reqBody, _ := ioutil.ReadAll(r.Body)
	var id int
	err := json.Unmarshal(reqBody, &id)
	if err != nil {
		fmt.Fprintf(w, "Unable to unmarshal json body: %s", err)
		return
	}

	temp := servers.Find(id)

	if temp == nil {
		fmt.Fprintf(w, "Server does not exist!")
		w.WriteHeader(http.StatusNotFound)
		return
	}

	json.NewEncoder(w).Encode(temp.Value)
}

func startServer(w http.ResponseWriter, r *http.Request) {
	reqBody, _ := ioutil.ReadAll(r.Body)
	var id int
	err := json.Unmarshal(reqBody, &id)
	if err != nil {
		fmt.Fprintf(w, "Unable to unmarshal json body: %s", err)
		return
	}

	temp := servers.Find(id)

	if temp == nil {
		fmt.Fprintf(w, "Server does not exist!")
		w.WriteHeader(http.StatusNotFound)
		return
	}
	serverInfo := temp.Value.(serverData)

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

func stopServer(w http.ResponseWriter, r *http.Request) {
	reqBody, _ := ioutil.ReadAll(r.Body)
	var id int
	err := json.Unmarshal(reqBody, &id)
	if err != nil {
		fmt.Fprintf(w, "Unable to unmarshal json body: %s", err)
		return
	}

	temp := runningServers.Find(id)

	if temp == nil {
		fmt.Fprintf(w, "Server does not exist!")
		w.WriteHeader(http.StatusNotFound)
		return
	}
	serverInfo := temp.Value.(runningServer)

	stdin := serverInfo.Stdin
	go func() {
		defer stdin.Close()
		io.WriteString(stdin, "stop\n")
	}()
}

func showLog(w http.ResponseWriter, r *http.Request) {
	reqBody, _ := ioutil.ReadAll(r.Body)
	var id int
	err := json.Unmarshal(reqBody, &id)
	if err != nil {
		fmt.Fprintf(w, "Unable to unmarshal json body: %s", err)
		return
	}

	temp := runningServers.Find(id)

	if temp == nil {
		fmt.Fprintf(w, "Server does not exist!")
		w.WriteHeader(http.StatusNotFound)
		return
	}
	serverInfo := temp.Value.(runningServer)

	fmt.Fprintf(w, "%s", serverInfo.Log.getLog())
}

func listServers(w http.ResponseWriter, r *http.Request) {
	encoder := json.NewEncoder(w)

	for e := servers.servers.Front(); e != nil; e = e.Next() {
		encoder.Encode(e.Value)
	}
}

func listRunningServers(w http.ResponseWriter, r *http.Request) {
	encoder := json.NewEncoder(w)

	if runningServers.servers.Len() == 0 {
		fmt.Fprintf(w, "{}")
		return
	}

	for e := runningServers.servers.Front(); e != nil; e = e.Next() {
		encoder.Encode(e.Value)
	}
}

func createServer(w http.ResponseWriter, r *http.Request) {
	reqBody, _ := ioutil.ReadAll(r.Body)
	var server serverData
	err := json.Unmarshal(reqBody, &server)
	if err != nil {
		fmt.Fprintf(w, "Unable to unmarshal json body: %s", err)
		return
	}

	servers.servers.PushBack(server)

	fmt.Fprintf(w, "Server created")
}
