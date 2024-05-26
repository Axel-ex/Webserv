## Usefull info 
I applied a chain of responsability pattern for the handling of the request. when we build a Response from a Request, we set a chain of handlers by connceting them together and we pass the request to the first one:

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
the handlers are linked together and if there are not able to process the request, they will pass it on to the next handler. If none of the handlers was able to handle the request, a BAD_REQUEST response is send:

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

