#!/bin/bash

# Variáveis de ambiente comuns em CGI
REQUEST_METHOD=${REQUEST_METHOD:-"GET"}
QUERY_STRING=${QUERY_STRING:-""}
CONTENT_LENGTH=${CONTENT_LENGTH:-0}
CONTENT_TYPE=${CONTENT_TYPE:-""}
DOCUMENT_ROOT=${DOCUMENT_ROOT:-"/var/www"}
SCRIPT_NAME=${SCRIPT_NAME:-"/cgi-bin/test.sh"}
REQUEST_URI=${REQUEST_URI:-"/cgi-bin/test.sh?name=example&age=25"}
SERVER_PROTOCOL=${SERVER_PROTOCOL:-"HTTP/1.1"}
REMOTE_ADDR=${REMOTE_ADDR:-"127.0.0.1"}

# Função para decodificar a query string
decode_url() {
    echo -e "$(echo "$1" | sed 's/+/ /g; s/%\(..\)/\\x\1/g;')"
}

# Processando a query string
name=""
age=""
if [ "$REQUEST_METHOD" == "GET" ] && [ -n "$QUERY_STRING" ]; then
    for param in ${QUERY_STRING//&/ }; do
        key=$(echo $param | cut -d '=' -f 1)
        value=$(echo $param | cut -d '=' -f 2)
        case "$key" in
            "name") name=$(decode_url "$value") ;;
            "age") age=$(decode_url "$value") ;;
        esac
    done
fi

# Cabeçalhos da resposta HTTP
echo -e "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n"

# Corpo da resposta
echo "<html>"
echo "<head><title>CGI Script Response</title></head>"
echo "<body>"
echo "<h1>CGI Script Response</h1><br><br>"

if [ -n "$name" ] && [ -n "$age" ]; then
    echo "<p>Hello, <b>$name</b>! You are <b>$age</b> years old.</p>"
else
    echo "<p>Hello, stranger! Please provide your <b>name</b> and <b>age</b> in the query string.</p>"
fi

echo "<h2>Environment Variables:</h2>"
echo "<ul>"
echo "<li>REQUEST_METHOD: $REQUEST_METHOD</li>"
echo "<li>QUERY_STRING: $QUERY_STRING</li>"
echo "<li>DOCUMENT_ROOT: $DOCUMENT_ROOT</li>"
echo "<li>SCRIPT_NAME: $SCRIPT_NAME</li>"
echo "<li>REQUEST_URI: $REQUEST_URI</li>"
echo "<li>SERVER_PROTOCOL: $SERVER_PROTOCOL</li>"
echo "<li>REMOTE_ADDR: $REMOTE_ADDR</li>"
echo "</ul>"

echo "</body>"
echo "</html>"
