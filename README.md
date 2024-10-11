# About 

Webserv is a lightweight web server developed entirely from scratch using C++. This project was designed with the primary goal of deepening the understanding of low-level network programming and server implementation. Built with socket programming at its core, Webserv handles client connections directly, managing request parsing, response generation, and connection lifecycle following c++98 standards.

Webserv supports a variety of key HTTP methods, including GET, POST, and DELETE, providing essential functionality for interacting with web clients. Additionally, it supports Common Gateway Interface (CGI) scripts, enabling dynamic content generation and server-side processing.

Inspired by the flexibility of Nginx, Webserv allows users to define server behavior and routing through a customizable configuration file. This configuration determines how requests are processed, enabling the server to handle multiple routes, serve static files, and execute CGI scripts.


### Usage

git clone this project cd in the root of the project:

```
cmake -B ./build

cmake --build ./build
```

it will create the executables in the build folder. our executable can be executed as so:

```./build/webserv```

### Key features
- Socket Programming: Direct management of client connections, request parsing, and response generation using low-level socket APIs.
- HTTP Methods Support: Handles essential HTTP methods (GET, POST, DELETE), making it suitable for various web applications.
- CGI Support: Executes CGI scripts, enabling dynamic content generation and server-side processing.
- Nginx-like Configuration: Flexible configuration file for routing, resource management, and error handling, similar to Nginx.


### Implementation

- **Logging:** A singleton `Log` class manages console output with various verbosity levels. It supports hierarchical log levels such as `ERROR`, `WARNING`, `INFO`, and `DEBUG`. Log entries include timestamps and are color-coded based on severity.

- **Parsing:** The `Parser` class parses the configuration file by tokenizing its content and performing synthax check on the tokens. It then stores settings such as ports, resource paths, and error pages among others in Config objects to then create the servers.

- **Cluster:** The `Cluster` class stores the servers and the file descriptor to poll. Upon receiving an event, it will match the right instance of the a Server object and call the `acceptIncomingConnection()` to create a new file descriptor for client server communication. 

- **Server Class:** The `Server` class encapsulates the core logic, from initialization (socket creation, binding) to client handling (accepting connections, serving requests). It integrates all components, ensuring smooth server operation.

- **Request Handling:** Requests are encapsulated within a `Request` object, which breaks down the HTTP request into manageable components (e.g., method, resource, headers, body). The server processes these requests efficiently and accurately.

- **Request Buffering:** The `RequestBuffer` class manages partial requests received over multiple reads, ensuring that the entire request is received before processing.

- **Chain of Responsibility:** Request handling follows the Chain of Responsibility pattern, where different handlers (e.g., `CgiRequestHandler`, `GetRequestHandler`) process requests based on their type. If a handler cannot process a request, it passes it to the next handler in the chain.

