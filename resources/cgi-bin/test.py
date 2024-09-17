#!/usr/bin/env python3

import os
import cgi
import html

# Função para gerar uma linha de tabela HTML
def html_table_row(header, value):
    return f"<tr><th>{html.escape(header)}</th><td>{html.escape(value)}</td></tr>"

# Início da resposta HTTP
print("HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n")

# Gera o conteúdo HTML
print("<html>")
print("<head>")
print("<title>CGI Script Response</title>")
print("</head>")
print("<body>")
print("<h1>CGI Script Response</h1>")
print("<h2>Environment Variables</h2>")
print("<table border='1'>")

# Exibe as variáveis de ambiente
for key, value in sorted(os.environ.items()):
    print(html_table_row(key, value))

print("</table>")

print("<h2>Query String Data</h2>")

# Análise da query string
form = cgi.FieldStorage()
if not form:
    print("<p>No query string data received.</p>")
else:
    print("<table border='1'>")
    for key in form.keys():
        value = form.getvalue(key)
        print(html_table_row(key, value))
    print("</table>")

print("<h2>Form Submission (POST Data)</h2>")

# Verifica se há dados de formulário POST
if os.environ.get("REQUEST_METHOD", "") == "POST":
    form_data = cgi.FieldStorage()
    if not form_data:
        print("<p>No POST data received.</p>")
    else:
        print("<table border='1'>")
        for key in form_data.keys():
            value = form_data.getvalue(key)
            print(html_table_row(key, value))
        print("</table>")
else:
    print("<p>No POST data. This was a GET request.</p>")

print("</body>")
print("</html>")
