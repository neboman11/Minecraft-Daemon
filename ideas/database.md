# Database

## Status - Implemented

Currently only sqlite3 is supported as a backend.

The current table schema is as follows:

| ID (INT PRIMARY KEY) | NAME (TEXT NOT NULL) | DIRECTORY (TEXT NOT NULL)   | JARFILE (TEXT NOT NULL) |
|----------------------|----------------------|-----------------------------|-------------------------|
| 1                    | test                 | /var/lib/jenkins/testserver | server.jar              |

## Description

The daemon should interface with a database to store information about servers (name, folder, jar, RAM allocation, etc.).

## Goals

- Removal of a server
- Store memory settings for the servers
- Store custom java settings for the servers
