# Minecraft-Daemon

[![Build Status](https://treeman.nesbitt.rocks/jenkins/job/Minecraft-Daemon/job/master/badge/icon)](https://treeman.nesbitt.rocks/jenkins/job/Minecraft-Daemon/job/master/)
[![codecov](https://codecov.io/gh/neboman11/Minecraft-Daemon/branch/master/graph/badge.svg?token=RHufYVW2ii)](https://codecov.io/gh/neboman11/Minecraft-Daemon)

 Open source multicraft clone

## Building

To build the daemon simply run `./configure && make`.

Debug builds can be make by setting the `DEBUG_BUILD` environment variable when running `autogen.sh` (requires `autoconf` to be installed) or `configure`.
(Note: `autogen.sh` and `configure` only check if `DEBUG_BUILD` is set, not what the value is.)
