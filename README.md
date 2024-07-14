# Documentation

### Compilation

first do in the root of the project:

```cmake -B ./build```

it will create a lock file with info needed by Cmake to build the project. Then:

``` cmake --build ./build```

it will create the executables in the build forlder. our executable can be executed as so:

```./build/webserv```

When you write your code if you need to add file for the compilation you can add the name of your file in the CMakeLists.txt (located at the root of the directory). there is a variable that stores the sources exactly like in a Makefile. Just add the name of your file there.
The cmake also compiles tests, i am using googletest library for unit testing. If you want to write tests you can add them in test.cpp (there is a dummy exemple). to run test you need to build first and then execute:

``` ./build/test/webserv_test```

test folder as its own CMakeLists.txt and if you want to create more testfile you can do like explained above: add  the name of the file you created to the CMakeLists.txt (this one located in the test folder).


### Classes

### log

#### Overview

Log is a Singleton class responsible for printing nicely formated output in the console.
This object has an internal state that governs the verbosity of our logging output, which can be set with the method Log::setLogLevel(). The log levels are organized in a hierarchical structure, where setting a specific log level enables logging at that level and also includes all levels beneath it in the hierarchy.

exemple:

The following exemple shows how the level is initially set in the Server constructor:
```cpp
typedef enum s_log_level
{
    ERROR,
    WARNING,
    INFO,
    DEBUG,
} e_log_level;

Server ::Server(std::string config_file)
{
    Config::parseFile(config_file);
    Log::setLogLevel(INFO);
    Log::clearScreen();
    std::signal(SIGINT, sigHandler);
}
```

#### Methods

- `static void setLogLevel(e_log_level level)` : set the log level. this method should be called once at the beginning of the program.
- `static void log(e_log_level level, const std::string& msg)`: Log a statement on the console with timestamp and right color according to the log level. Once again, calling this function with a log level higher then the one set will just have no effect.
- `static void logRequest(const Request &request)`: Log request information.
- `static void clearScreen(void)`: clears the screen at the beginning of the program.


### Config

#### Overview

Config is another Singleton class that stores all the information parsed from the config file the program receives as argument. It provides an interface to access this information. It stores info relative to the ports we are listening on, and a map of the resources where a path (ex: "/form2") is associated with an html ressource. Additionally it stores errors which are simply a map of an error code and a t_error which is a small structure that contains an html string and a reason (ex: Not found, bad request....)

#### Methods

- `static void parseFile(std::string file)`: This method will contain all the parsing logic. for now it contain a dummy logic. It will mainly have to store the resource we are going to serve to the client.
- `static std::vector<int> &getPorts(void)`: getter for the ports we listen on.
- `static std::map<std::string, std::string> &getResources(void)`: getter for the page to be served. 
- `static std::map<int, t_error> &getErrors(void)`: getter for the errors.

### Request

The request is simply an object that store request information in a tidy way which make the handling process way easier. i just decided to divide the request into its sub component and store it into an object.

- `Request(std::string buffer)`: construct a request from a string buffer.
- `const std::string& getMethod(void) const`: getter for method (POST, GET, DELETE).
- `const std::string& getResource(void) const`: getter for request resource (/, /form2 ...).
- `const std::string& getProtocol(void) const`: getter for protocol (we should only handle HTTP/1.1).
- `const std::string& getHeaders(void) const`: getter for the headers.
- `const std::string& getBody(void) const`: getter for the body.

### RequestBuffer

#### Overview

Since a request can be heavy and we are using a fixed sized buffer to receive it, the are chances the request will not be complete in a single read call. I added a RequestBuffer class (in Request.hpp) which keeps track of what it has receive so far by calculating the length of the request body and comparing it with how many byte it received so far. If the two values match it will set it's internal flag "_request_over" to true.
in the server loop to serve client, we will loop on the request until we receive it entirely. We also set a TIMEOUT not to stay eternally in this loop if anything wrong happens:

```cpp
// Read from client chunks. set a TIMEOUT for long request.
while (!request_buffer.isRequestOver())
{
	clock_t curr = clock();
	double elapsed =
		static_cast<double>(curr - start) / CLOCKS_PER_SEC;
	if (elapsed > SERVER_TIMEOUT)
		break;

	std::memset(read_buffer, 0, sizeof(read_buffer));
	ssize_t n = _readFd(i, read_buffer, sizeof(read_buffer));
	if (n < 0)
		continue;
	request_buffer.appendBuffer(read_buffer, n);
}
```

#### Methods

- `bool isRequestOver(void) const`: retrun true of the request is over.
- `void appendBuffer(const std::string &buffer, size_t size)`: append the buffer passed to the request and update the content length.
- `std::string &getBuffer(void)`: getter for the request buffer.
- `size_t getContentLength(void) const`: 
- size_t getCurrentLength(void) const;

### Request handlers

#### Overview

I applied a chain of responsability pattern for the handling of the request. when we build a Response from a Request, we set a chain of handlers by connecting them together and we pass the request and the reponse object we are trying to build (*this) to the first handler:

```cpp
Response ::Response(const Request& request)
{
    /* The request handling is design as a "chain of responsability" pattern*/
	/* https://www.geeksforgeeks.org/chain-responsibility-design-pattern/ */
    /* Create handlers for each request type */
    CgiRequestHandler cgi_handler;
    GetRequestHandler get_handler;
    PostRequestHandler post_handler;
    DeleteRequestHandler delete_handler;

    /* Link the handlers together */
    cgi_handler.setNextHandler(&get_handler);
    get_handler.setNextHandler(&post_handler);
    post_handler.setNextHandler(&delete_handler);

    /* Pass the request to the handler of the chain */
    cgi_handler.handleRequest(request, *this);
}
```
the handlers are linked together and are responsible for building the response. If the handler receving the request is not able to process the request, it will pass it on to the next handler. If none of the handlers was able to handle the request, a BAD_REQUEST response is send:

```cpp
void ARequestHandler ::handleRequest(const Request& request, Response& response)
{
    if (request.getProtocol() != "HTTP/1.1")
        return createErrorResponse(BAD_REQUEST, response);

    if (canProcess(request))
        processRequest(request, response);
    else if (_next)
        _next->handleRequest(request, response);
    else
        createErrorResponse(BAD_REQUEST, response);
}
```

We now if needed can add handlers easily without having to modify the code and our code adhers to the single responsability principle.

#### Methods

- `void createErrorResponse(int error_code, Response& response) const`: populate the response with the appropriate error page.
- `void createOkResponse(std::string resource, Response& response) const`: populate the response with appropriate OK response
- `virtual bool _canProcess(const Request& request) const`: abstract method that defines if the handler can process the request (ex: GetRequestHandler will override this method and simply check if the request.getMethod() == "GET").
- `virtual void processRequest(const Request& request, Response& response) const = 0`: abstract method specific to each handler. this method contains the logic that will be different for each handler.

### Server

#### Overview

Finally this class is the class that encapsulate all the logic of the program.

#### Methods

Public:

- `Server(std::string config_file)`: constructor, call the parse, set log level, set signal handler.
- `void init(void)`: Init the server with the information parse in the Config, creates and bind the socket.
- `void start(void)`: start the server (starts polling the fd bind to the socket)
- `void _acceptIncomingConnections(void)`: open a fd to communicate with client if activity on our port is detected.
- `void _serveClients(void)`: contain all the logic relative to parsing the request and creating a response
- `ssize_t _readFd(int fd, char *buffer, size_t buffer_size)`: populate the buffer with the information read from fd.


## AND SO?
Now we can navigate to localhost:port_nb/form2 and choose a file to upload. it works well except when file don't have extension / compressed files. a file can be posted with different type of format, i am handeling the case of multipart/form-data so it fails handeling anything else. for instance "/form" will send POST request encoded in an other format so the server will respond to you that it failed pasring the request.
so we can basically request 3 resources so far ("/", "/form", "form2") and post through /form2 anyfile with valid extensions.

## TODO
- parsing config file into the config
- handling cgi
- handling DELETE request
- Check if we need to handle POST when its not send as multipart/form-data
- think about how we could have some JS in our html templates to navigate through hyperlinks / delete file through a GUI.

