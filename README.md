# Minecraft-Daemon

[![Build Status](https://treeman.nesbitt.rocks/jenkins/job/Minecraft-Daemon/job/master/badge/icon)](https://treeman.nesbitt.rocks/jenkins/job/Minecraft-Daemon/job/master/)
[![codecov](https://codecov.io/gh/neboman11/Minecraft-Daemon/branch/master/graph/badge.svg?token=RHufYVW2ii)](https://codecov.io/gh/neboman11/Minecraft-Daemon)

 Open source multicraft clone

## Usage

### Prerequisites

Minecraft-Daemon requires a database to store information in. Currently the only supported database system is MySQL/MariaDB.

A configuration file must also be provided. An example can be found [here](https://github.com/neboman11/Minecraft-Daemon/blob/master/minecraft-daemon.toml.example).

### Running

After filling the configuration file with the desired values, place it in the same folder as the `Minecraft-Daemon` executable. Then simply run the executable.

## Building

To build and run:

```sh
git clone https://github.com/neboman11/Minecraft-Daemon
cd Minecraft-Daemon
go build
./Minecraft-Daemon
```
