# Multi-threaded DNS Client (C)

A high-performance DNS resolver utility implemented in C for Windows.

## Project overview

This tool performs direct UDP DNS lookups for A and AAAA records using a custom packet serialization and deserialization pipeline.

- Multi-threaded domain resolution with parallel UDP queries
- Full DNS protocol implementation from request construction to response parsing
- Supports both IPv4 and IPv6 DNS queries
- Windows socket-based networking with retry logic for resilient lookups

## Build

Requirements:

- GCC toolchain (e.g. MinGW)
- Windows sockets library (`ws2_32`)

Build with:

```cmd
make
```

This produces `DNS_Client.exe`.

## Run

Execute the program and enter the path to a text file containing domain names:

```cmd
DNS_Client.exe
```

Then choose:

- `1` for IPv4 resolution
- `2` for IPv6 resolution

The client resolves each entry in the file concurrently.

## Clean

Remove build artifacts with:

```cmd
make clean
```
