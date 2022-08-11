package main

import (
	"container/list"
	"io"
)

// ServerLog is the last 100 lines of the server's log
type ServerLog struct {
	ID      int
	Len     int
	Log     *list.List
	stdout  io.ReadCloser
	channel chan string
}

// Init initializes or clears list l.
func (l *ServerLog) Init(stdout io.ReadCloser) *ServerLog {
	l.Log = list.New()
	l.stdout = stdout
	l.channel = make(chan string)
	return l
}

// NewServerLog returns an initialized serverlog.
func NewServerLog(stdout io.ReadCloser) *ServerLog { return new(ServerLog).Init(stdout) }

func (l ServerLog) readLog() {
	buffer := make([]byte, 1024)

	for {
		_, err := l.stdout.Read(buffer[:])
		if err != nil {
			break
		}

		line := trimNewLine(buffer)

		l.Log.PushBack(line)
		l.Len++
		select {
		case l.channel <- line:
		default:
		}

		if l.Len > 100 {
			l.Log.Remove(l.Log.Front())
			l.Len--
		}
	}
}

func (l ServerLog) GetLog() string {
	var log string

	for e := l.Log.Front(); e != nil; e = e.Next() {
		log += string(e.Value.(string))
	}

	return log
}

func trimNewLine(s []byte) string {
	line := ""

	for i := 0; i < len(s); i++ {
		line += string(s[i])

		if s[i] == '\n' {
			break
		}
	}

	return line
}

func (l ServerLog) GetLogChannel() chan string {
	return l.channel
}
