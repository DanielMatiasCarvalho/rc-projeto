# RC

# User client

The client can be called using:

```./user [-n ASIP] [-p ASport]```

The default ip will be `localhost` and the default port will be `58085`.

# Auction Server

The auction server can be called using:

```./AS [-vr] [-p ASport] [-d DBpath]```

The default port will be `58085`.

The flag `-v` starts the server in verbose mode, in which all user-server interactions will be logged.
The flag `-r` will wipe the database on server start. This can be used for testing.
The option `-d DBpath` will indicate a different path to the `db` directory. The default name will be `database`.