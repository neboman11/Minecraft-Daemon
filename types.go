package main

import (
	"container/list"
	"io"

	"github.com/uptrace/bun"
)

type ServerStatus int

const (
	Running ServerStatus = iota
	Stopped
)

type databaseServer struct {
	bun.BaseModel `bun:"table:servers"`

	ID          int64  `bun:"id,pk,autoincrement"`
	Name        string `bun:"name,notnull"`
	Directory   string `bun:"directory,notnull,unique"`
	JarFile     string `bun:"jar_file,notnull"`
	RunMemory   string `bun:"run_memory,notnull"`
	StartMemory string `bun:"start_memory,notnull"`
	JavaArgs    string `bun:"java_args"`
}

type responseServer struct {
	ID          int64        `json:"id"`
	Name        string       `json:"name"`
	Directory   string       `json:"directory"`
	JarFile     string       `json:"jar_file"`
	RunMemory   string       `json:"run_memory"`
	StartMemory string       `json:"start_memory"`
	JavaArgs    string       `json:"java_args"`
	Status      ServerStatus `json:"status"`
}

type requestServer struct {
	Name        string `json:"name"`
	Directory   string `json:"directory"`
	JarFile     string `json:"jar_file"`
	RunMemory   string `json:"run_memory"`
	StartMemory string `json:"start_memory"`
	JavaArgs    string `json:"java_args"`
}

type runningServer struct {
	ID    int64
	Stdin io.WriteCloser
	Log   *ServerLog
}

type runningServerList struct {
	servers *list.List
}

func (l runningServerList) Find(i int64) *list.Element {
	for e := l.servers.Front(); e != nil; e = e.Next() {
		if e.Value.(runningServer).ID == i {
			return e
		}
	}

	return nil
}
