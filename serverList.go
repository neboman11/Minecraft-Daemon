package main

import (
	"container/list"
	"io"
)

type requestServer struct {
	Name        string `json:"name"`
	Directory   string `json:"directory"`
	JarFile     string `json:"jar_file"`
	RunMemory   string `json:"run_memory"`
	StartMemory string `json:"start_memory"`
	JavaArgs    string `json:"java_args"`
}

type runningServer struct {
	ID    int
	Stdin io.WriteCloser
	Log   *ServerLog
}

type runningServerList struct {
	servers *list.List
}

func (l runningServerList) Find(i int) *list.Element {
	for e := l.servers.Front(); e != nil; e = e.Next() {
		if e.Value.(runningServer).ID == i {
			return e
		}
	}

	return nil
}

func newRunningServerList() *runningServerList {
	return &runningServerList{list.New()}
}

var runningServers *runningServerList
