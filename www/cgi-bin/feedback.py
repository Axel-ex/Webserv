#!/usr/bin/env python

import cgi

print("HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n")

print("""
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="stylesheet" href="/assets/css/style.css">
    <link rel="stylesheet" href="/assets/css/navbar.css">
    <link rel="stylesheet" href="/assets/css/banner.css">
    <title>Feedback Form</title>
</head>

<body>
    <div class="banner"></div>

    <div id="mySidenav" class="sidenav">
        <div style="padding-bottom: 50px;">
            <a id="closebtn" href="javascript:void(0)" class="closebtn" onclick="closeNav()">&times;</a>
        </div>
        <div class="pages">
            <a href="index.html">Home</a>
            <a href="server_architecture.html">Server Architecture</a>
            <a href="http_protocol.html">Understanding HTTP</a>
            <a href="socket.html">Socket Programming</a>
            <a href="request_handling.html">Request Handling</a>
            <a href="cgi_handler.html">CGI Implementation</a>
            <a href="logging.html">Logging and Monitoring</a>
        </div>
    </div>

    <span id="openNav" onclick="openNav()" ;>&#9776;</span>

    <div id="main">
        <h1>Give Us Your Feedback</h1>
        <form method="post" action="/cgi-bin/feedback.py">
            <label for="name">Name:</label><br>
            <input type="text" id="name" name="name"><br><br>
            <label for="comments">Comments:</label><br>
            <textarea id="comments" name="comments"></textarea><br><br>
            <input type="submit" value="Submit">
        </form>
    </div>
""")

form = cgi.FieldStorage()

if "name" in form and "comments" in form:
    name = form["name"].value
    comments = form["comments"].value
    print(f"""
    <div id="main">
        <h2>Thank you, {name}!</h2>
        <p>Your comments: {comments}</p>
    </div>
    """)

print("""
    <script src="/assets/js/script.js"></script>
</body>

</html>
""")
