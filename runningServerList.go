package main

import "container/list"

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

func (l runningServerList) Remove(i int64) {
	e := l.Find(i)
	if e != nil {
		l.servers.Remove(e)
	}
}
