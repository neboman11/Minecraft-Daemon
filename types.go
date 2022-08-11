package main

import (
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
	Restart     bool   `bun:"restart,notnull"`
}

type responseServer struct {
	ID          int64        `json:"id"`
	Name        string       `json:"name"`
	Directory   string       `json:"directory"`
	JarFile     string       `json:"jar_file"`
	RunMemory   string       `json:"run_memory"`
	StartMemory string       `json:"start_memory"`
	JavaArgs    string       `json:"java_args"`
	Restart     bool         `json:"restart"`
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
	ID      int64
	Stdin   io.WriteCloser
	Log     *ServerLog
	Restart bool
}
