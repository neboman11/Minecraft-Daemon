# Server Logs: Memory vs File

## Status - Implemented

Current implementation is memory.

## Memory

### Memory Pros

- much faster access times
- no file handling necessary
- can create special data structure to hold it for easy access/manipulation

### Memory Cons

- higher memory usage
- need to design special data structure for it

### Possible Application

Create special data structure that will hold x number of lines of text
For every line read past x lines, the top most/first line is deleted (FIFO)
When requested, data structure is passed to requester

### Questions (and possible answers)

How are updates handled for requesters? (Requesters have keep-alive on if the data is being viewed and requestee pushes new lines while keep-alive is active)

## File

### File Pros

- less memory usage
- log files already exist (no need to create one and use more space)

### File Cons

- knowing when a new line is added to the log
- slower access times
- need to keep track of file directories (possible differing structures for different server versions
