package main

import (
	"container/list"
	"io"
)

type runningServerList struct {
	servers *list.List
}

func newRunningServerList() *runningServerList {
	return &runningServerList{list.New()}
}

func (l runningServerList) Add(id int64, stdin io.WriteCloser, log *ServerLog, restart bool) {
	runningServers.servers.PushBack(runningServer{id, stdin, log, restart})
}

func (l runningServerList) Find(i int64) *list.Element {
	for e := l.servers.Front(); e != nil; e = e.Next() {
		if e.Value.(runningServer).ID == i {
			return e
		}
	}

	return nil
}

func (l runningServerList) Remove(i int64) {
	e := l.Find(i)
	if e != nil {
		l.servers.Remove(e)
	}
}

func (l runningServerList) Length() int {
	return l.servers.Len()
}

func (l runningServerList) Front() *list.Element {
	return l.servers.Front()
}

var runningServers *runningServerList
