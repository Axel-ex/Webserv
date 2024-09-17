# Socket Programming in C++: Building a Simple Server

Socket programming is the foundation of network communication in many low-level applications. This article explains the core concepts involved in building a socket server using C++ and POSIX system calls. The server listens for incoming client connections, manages multiple clients using `poll()`, and reads/writes data to/from these clients.

In this guide, we’ll walk through the creation, binding, and usage of sockets in a multi-client scenario.

---

## 1. Creating a Socket

The first step in socket programming is to create a socket using the `socket()` system call. Sockets are essentially network endpoint that establishes a connection for sending and receiving data between programs. The call to `socket()` returns a file descriptor (FD) that we can use to refer to the socket.

```cpp
int sockfd = socket(AF_INET, SOCK_STREAM, 0);
if (sockfd < 0)
    throw ServerError("Fail creating socket");
```

- AF_INET: This indicates the use of IPv4. For IPv6, you would use AF_INET6.
- SOCK_STREAM: This specifies that the socket will be a TCP stream.
- Protocol (0): Typically, 0 is used to choose the default protocol for the socket type, which is TCP for SOCK_STREAM.

## 2. Binding the Socket to an Address

After creating a socket, it must be bound to an IP address and port number using the `bind()` function. This is achieved using the `t_sockaddr_in` structure which give the user the ability to assigns a name (IP and port) to the socket.

```cpp
t_sockaddr_in address;
std::memset(&address, 0, sizeof(address));
address.sin_family = AF_INET;
address.sin_addr.s_addr = INADDR_ANY;
address.sin_port = htons(Config::getPorts()[i]);

if (bind(sockfd, (t_sockaddr *)&address, sizeof(address)) < 0)
    throw ServerError("Fail binding the socket");
```

- sin_family: The address family (AF_INET for IPv4).
- sin_addr.s_addr: The IP address to bind the socket to (INADDR_ANY binds to all available interfaces).
- sin_port: The port number (converted to network byte order using `htons()`).

## 3. Listening for Connections

Once the socket is bound, the server listens for incoming connections using the `listen()` system call:

```cpp
if (listen(sockfd, MAX_CLIENT) < 0)
    throw ServerError("fail listening for connection");
```
The `listen()` function marks the socket as a passive socket that will be used to accept incoming connection requests. The backlog parameter defines the maximum number of connections that can be waiting to be accepted. If the queue is full, new connection attempts may be refused or ignored.

## 4. Polling for Multiple Clients

Handling multiple client connections simultaneously is a challenge. This server uses `poll()` to monitor multiple file descriptors (both server sockets and client connections) for incoming data.

```cpp
activity = poll(_fds.data(), _fds.size(), 1000);
```

- `poll()`: Monitors multiple file descriptors for events, such as data to read or new incoming connections.
- _fds: A vector of file descriptors, including both server and client sockets.
- POLLIN: Event indicating there is data to read on the socket.

The `poll()` function checks (polls) each file descriptor in the _fds vector for events. It returns the number of file descriptors with events, or -1 if an error occurs. The timeout value (1000 milliseconds in this case) specifies how long `poll()` should wait for events before returning.


## 5. Accepting Incoming Connections

When a server socket detects an event (a new connection request), the server calls `accept()` to accept the client connection and create a new file descriptor for that client.

```cpp
int newfd = accept(_fds[i].fd, NULL, NULL);
if (newfd < 0 && !stopFlag)
    throw ServerError("Error accepting connection");

t_pollfd new_pollfd;
new_pollfd.fd = newfd;
new_pollfd.events = POLLIN;
_fds.push_back(new_pollfd);  // Add new client to poll list
```
accept(): Accepts a connection from a client and returns a new file descriptor that refers to the client’s connection.
new_pollfd: A pollfd structure for the newly accepted client connection.
POLLIN: Event indicating that there is data to read on the new client socket.

The `accept()` function extracts the first connection request on the queue of pending connections, creates a new socket for the connection, and returns a file descriptor for that socket. This new file descriptor is added to the _fds vector so that the server can monitor it for incoming data.

## 6. Reading Data from Clients

After a client is connected, the server reads data using `recv()`. This function receives data from the specified file descriptor.

```cpp
ssize_t n = recv(_fds[fd_index].fd, buffer, buffer_size, 0);
if (n < 0) {
    close(_fds[fd_index].fd);
    _fds.erase(_fds.begin() + fd_index);
    Log::log(ERROR, "reading client request");
}
```

- `recv()`: Reads data from the client into the buffer. It returns the number of bytes read or -1 on failure.
- `close()`: If the client disconnects or an error occurs, the socket is closed, and the file descriptor is removed from the polling list.
- buffer: A buffer to store the received data.
- buffer_size: The size of the buffer.

The `recv()` function is used to read data from the client. If an error occurs during the read operation, the server closes the connection and removes the file descriptor from the list of monitored file descriptors. Proper error handling ensures that the server can manage client disconnections and failures gracefully.


## 7. Closing Connections

After sending the response, the server closes the connection with the client:

```cpp
close(_fds[i].fd);
_fds.erase(_fds.begin() + i);
```

- `close()`: Closes the connection, freeing the file descriptor.
- `_fds.erase()`: Removes the client's file descriptor from the polling list.

The `close()` function is used to close the socket associated with a client connection. This frees up the file descriptor and ensures that the server can handle new connections. After closing the connection, the file descriptor is removed from the _fds vector to stop monitoring it for further events.


## Conclusion

In summary, socket programming is a powerful technique for building networked applications. Socket programming essentially comes down to:

- **Create and configure sockets**: Setting up the basic communication channels using `socket()`, configuring options with `setsockopt()`, and binding to addresses and ports.
- **Listen for incoming connections**: Using `listen()` to wait for client connection requests.
- **Manage multiple clients**: Leveraging `poll()` to handle multiple file descriptors efficiently.
- **Accept and handle client connections**: Accepting new connections with `accept()`, reading data with `recv()`, and responding to clients.
- **Close connections**: Properly closing and removing file descriptors to manage resources effectively.

