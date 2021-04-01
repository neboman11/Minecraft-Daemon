package main

import (
	"container/list"
)

func newRunningServerList() *runningServerList {
	return &runningServerList{list.New()}
}

var runningServers *runningServerList
