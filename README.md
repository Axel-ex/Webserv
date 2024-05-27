## Usefull info


#### log
In our logging system, we establish an internal state at the initiation of the program within the Server constructor. This internal state governs the verbosity of our logging output by allowing us to define a log level. The log levels are organized in a hierarchical structure, wherein setting a specific log level not only enables logging at that level but also includes all levels beneath it in the hierarchy.

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
    Log::setLoglevel(INFO);
    Log::clearScreen();
    std::signal(SIGINT, sigHandler);
}

```
For instance, if we set our log level to 'INFO', all logging operations with a severity level of 'INFO' or higher will be executed, including 'ERROR' and 'WARNING'. Conversely, if we set our log level to 'DEBUG', all logging operations, regardless of severity, will be executed. ERROR = only error message will be printed.

The log also contains static operations to log specific object (logRequest) or to print and format our output in specific way for the display of infos to be esthetic

#### RequestBuffer
Since a request can be heavy and we are using a fixed sized buffer to receive it, the are chances the request will not be complete in a single read call. I added a RequestBuffer class (in Request.hpp) which keeps track of what it has receive so far by calculating the length of the request body and comparing it with how many byte it received so far. If the two values match it will set it's internal flag "_request_over" to true.

```cpp
class RequestBuffer
{
    public:
        RequestBuffer(void);
        ~RequestBuffer(void);

		bool isRequestOver(void) const;
		void appendBuffer(const std::string &buffer, size_t size);
		std::string &getBuffer(void);

		size_t getContentLength(void) const;
		size_t getCurrentLength(void) const;

    private:
        std::string _buffer;
        size_t _content_length;
        size_t _current_length;
		bool _request_over;

		void _getContentLength(void);
};
```
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
#### Request handlers
I applied a chain of responsability pattern for the handling of the request. when we build a Response from a Request, we set a chain of handlers by connecting them together and we pass the request and the reponse to the first one:

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
the handlers are linked together and are responseible for building the response. If the handler receving the request is not able to process the request, it will pass it on to the next handler. If none of the handlers was able to handle the request, a BAD_REQUEST response is send:

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

## AND SO?
Now we can navigate to localhost:port_nb/form2 and choose a file to upload. it works well except when file don't have extension / compressed files. a file can be posted with different type of format, i am handeling the case of multipart/form-data so it fails handeling anything else. for instance "/form" will send POST request encoded in an other format so the server will respond to you that it failed pasring the request.
so we can basically request 3 resources so far ("/", "/form", "form2") and post through /form2 anyfile with valid extensions.

## TODO
- parsing config file into the config
- handling cgi
- handling DELETE request
- Check if we need to handle POST when its not send as multipart/form-data
- think about how we could have some JS in our html templates to navigate through hyperlinks / delete file through a GUI.
- maybe there's a smart way to create OK/Error response using only one html template that we could modify to insert error code, reason and personalised msg


## Compilation

first do in the root of the project:

```cmake -B ./build```

it will create a lock file with info needed by Cmake to build the project. Then:

``` cmake --build ./build```

it will create the executables. our executable can be executed as so:

```./build/webserv```

When you write your code if you need to add file for the compilation you can add the name of your file in the CMakeLists.txt at the root of the directory.
If you want to write tests you can add them in test.cpp (there is a dummy exemple). to run test you need to build first and then execute:

``` ./build/test/webserv_test```

test folder as its own CMakeLists.txt and if you want to create more testfile you can do like explained above: add  the name of the file you created to the CMakeLists.txt of the test folder.

