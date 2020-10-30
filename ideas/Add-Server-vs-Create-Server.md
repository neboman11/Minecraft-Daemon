# Add Server vs Create Server

## Add Server

Server has already been created elsewhere and is being added to be ran/tracked by the daemon.
Jar is already in server dir.
Daemon must move server into it's server store directory

### Pseudocode

#### GUI

```text
Add Server {
    Form {
        Server Name - string field
        Server Current Directory - string field
        Server Final Directory - string field (auto-populated by Server Name)
        Server Type - drop down(Vanilla, Spigot, Paper, Forge)
        Server Jar Location - radio button/drop down(Jar Store, Server Directory)
        Server Jar - if (Server Jar Location == Jar Store) -> drop down(list jar files from Server Type category)
                     if (Server Jar Location == Server Directory) -> string field
        Run Memory - int field + size drop down
        Start Memory - int field + size drop down
        Extra Java Args - string field
        Start Server - check box
    }

    submit form to Daemon
}
```

#### Daemon

```text
Add Server {
    move server folder from Server Current Directory to Server Final Directory
    add server to database
    if (Start Server == true) -> start the server
}
```

## Create Server

Server does not exist, no server folder or jars anywhere. Daemon must create all the necessary components to get it working.
Accept EULA automatically.

### Pseudocode

#### GUI

```text
Create Server {
    Form {
        Server Name - string field
        Server Directory - string field (auto-populated by Server Name)
        Server Type - drop down(Vanilla, Spigot, Paper, Forge)
        Server Jar - drop down(list jar files from Server Type category)
        Run Memory - int field + size drop down
        Start Memory - int field + size drop down
        Extra Java Args - string field
        Start Server - check box
    }

    submit form to Daemon
}
```

#### Daemon

```text
Create Server {
    add server to database
    create server directory
    write eula file
    if (Start Server == true) -> start the server
}
```
