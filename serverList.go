package main

import (
	"container/list"
	"io"
)

type serverData struct {
	ID          int    `json:"ID"`
	Name        string `json:"Name"`
	Directory   string `json:"Directory"`
	JarFile     string `json:"JarFile"`
	RunMemory   string `json:"RunMemory"`
	StartMemory string `json:"StartMemory"`
	JavaArgs    string `json:"JavaArgs"`
}

type runningServer struct {
	ID    int
	Stdin io.WriteCloser
	Log   *ServerLog
}

type serverList struct {
	servers *list.List
}

func (l serverList) Find(i int) *list.Element {
	for e := l.servers.Front(); e != nil; e = e.Next() {
		if e.Value.(serverData).ID == i {
			return e
		}
	}

	return nil
}

func newServerList() *serverList {
	return &serverList{list.New()}
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

var servers *serverList
var runningServers *runningServerList
