# WinPM

An extremely small windows process manager written
in C using pure WINAPI.

## Compilation

We use GCC under Window (Cygwin version), but you can try
to use any other C compiler.

```sh
make
```

## Usage

Display help:

```sh
winpm.exe help
```

Install a service:

```sh
winpm.exe install
```

Install a service as another user:

```sh
winpm.exe install <user> <password>
```

Uninstall service:

```sh
winpm.exe uninstall
```

Run all processes from configuration file:

```sh
winpm.exe run
```

## Configuration

Configuration file `winpm.cnf` must be located in the same directory
together with `winpm.exe`.

Format of file is a simple text file which represents list of commands
line by line. Symbol `#` means a start of inline comment.

Each command follows this format:

`SETTING -- CMD`

Where `SETTING` is a spare separated list of settings, which can contains:
- `cwd <directory>` - specify working directory for the process;
- `delay <msec>` - delay specified time before starting new process;
- `respawn` - mode which means "respawn the process if it is exited";
- `autostart` - mode which means "just run once and wait for the process";

`--` - is a separator between settings and command line.

`CMD` - is a command line for running of the process.

Examples:

```conf
# respawn command "node.exe compute.js" with delay in 3sec between executions
cwd "C:\Projects" respawn delay 3000 -- node.exe compute.js
# when winpm is started wait for 15s and run command "node.exe prepare.js" once
cwd "C:\Projects" autostart delay 15000 -- node.exe prepare.js
```
