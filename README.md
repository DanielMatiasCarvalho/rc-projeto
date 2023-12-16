# RC

# User client

The client can be called using:

```./user [-n ASIP] [-p ASport]```

- The option `-n ASIP` indicates the hostname of the auction server. The default name will be `127.0.0.1/localhost`.
- The option `-p ASport` indicates the port that the auction server will be listening to (in TCP and UDP). The default port will be `58085`.

The client offers a command line interface with complete interactive features. Users can edit the current line by using the left/right arrow keys. Users can use the tab to auto-complete file names aswell as using the up/down arrow keys to visit their command history on the current execution of the program.

# Auction Server

The auction server can be called using:

```./AS [-vr] [-p ASport] [-d DBpath]```

- The option `-p ASport` indicates the port that the server will be listening to (both in TCP and UDP). The default port will be `58085`.
- The flag `-v` starts the server in verbose mode, in which all user-server interactions will be logged.
- The flag `-r` will wipe the database on server start. This can be used for testing.
- The option `-d DBpath` will indicate a different path to the `db` directory. The default name will be `database`.