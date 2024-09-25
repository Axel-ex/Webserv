#!/usr/bin/env python3

import os
import cgi
import html
import sys
from urllib.parse import parse_qs

# Função para gerar uma linha de tabela HTML com escape de caracteres
def html_table_row(header, value):
    if isinstance(value, bytes):
        value = value.decode('utf-8', errors='replace')  # Use replace to handle non-decodable bytes
    return f"<tr><th>{html.escape(header)}</th><td>{html.escape(value)}</td></tr>"

# Função para gerar uma tabela a partir de um dicionário
def generate_table(data):
    table_html = "<table border='1'>"
    for key, value in sorted(data.items()):
        table_html += html_table_row(key, value)
    table_html += "</table>"
    return table_html

# Função para processar dados de formulário
def process_form_data(form):
    form_html = "<table border='1'>"
    for key in form.keys():
        field_item = form[key]
        if field_item.filename:
            # Trata upload de arquivo
            try:
                file_content = field_item.file.read()
                # Exibe tipo do arquivo
                form_html += html_table_row(key, f"{field_item.filename} ({field_item.type})")
                
                # Exibe conteúdo do arquivo se for texto
                if 'text' in field_item.type:
                    file_content_str = file_content.decode('utf-8', errors='replace')
                    form_html += html_table_row("File Content", file_content_str)
                else:
                    form_html += html_table_row("File Content", "Binary file uploaded; content not displayed.")
            except Exception as e:
                form_html += html_table_row("Error", f"Could not read file: {e}")
        else:
            # Campo de formulário comum
            value = form.getvalue(key)
            if isinstance(value, bytes):
                value = value.decode('utf-8', errors='replace')
            form_html += html_table_row(key, value)
    form_html += "</table>"
    return form_html

# Início da resposta HTTP
print("HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n")

# Geração do HTML de resposta
print(f"""
<!DOCTYPE html>
<html lang='en'>
<head>
    <meta charset='UTF-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <link rel='stylesheet' href='/assets/css/style.css'>
    <link rel='stylesheet' href='/assets/css/navbar.css'>
    <link rel='stylesheet' href='/assets/css/banner.css'>
    <title>CGI Script Response</title>
</head>
<body>
    <div class='banner'></div>
    <div id='mySidenav' class='sidenav'>
        <div style='padding-bottom: 50px;'>
            <a id='closebtn' href='javascript:void(0)' class='closebtn' onclick='closeNav()'>&times;</a>
        </div>
        <div class='pages'>
            <a href='/index.html'>Home</a>
            <a href='/server_architecture.html'>Server Architecture</a>
            <a href='/http_protocol.html'>Understanding HTTP</a>
            <a href='/socket.html'>Socket Programming</a>
            <a href='/request_handling.html'>Request Handling</a>
            <a href='/cgi_handler.html'>CGI Implementation</a>
            <a href='/logging.html'>Logging and Monitoring</a>
        </div>
    </div>
    <span id='openNav' onclick='openNav()'>&#9776;</span>
    <div id='main'>
        <h1>CGI Script Response</h1><br><br>
        <h2>Environment Variables</h2>
        <div class='center-table'>
""")

# Gera a tabela com as variáveis de ambiente


print(generate_table(os.environ))

print("</div>")

# Exibe os dados da query string, se houver
form = cgi.FieldStorage()

print("<h2>Form Submission (POST Data)</h2>")

if os.environ.get("REQUEST_METHOD", "") == "POST":
    if not form.keys():
        print("<p>No POST data received.</p>")
    else:
        print("<div class='center-table'>")
        print(process_form_data(form))
        print("</div>")
        print("<div class='center-table'><p>Test POST again: </p>")
        print("<p><a href='/upload.html'>Upload a file</a></p></div>")
else:
    print("<div class='center-table'><p>No POST data. This was a GET request.</p>")
    print("<p><a href='/upload.html'>Upload a file</a></p></div>")


# Display query string variables
print("<h2>Query String Variables</h2>")
query_string = parse_qs(os.environ.get("QUERY_STRING", ""))
if query_string:
    print("<div class='center-table'>")
    print("<table>")
    print("<tr><th>Variable</th><th>Value</th></tr>")
    for key, values in query_string.items():
        for value in values:
            print(f"<tr><td>{key}</td><td>{value}</td></tr>")
    print("</table>")
    print("</div>")
else:
    print("<div class='center-table'><p>No query string variables passed.</p>")



# Finaliza o HTML
print("""
    </div>
    <script src='/assets/js/script.js'></script>
</body>
</html>
""")
