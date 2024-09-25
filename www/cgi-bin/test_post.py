#!/usr/bin/env python3

import cgi
import html
import os
import sys

print("Content-Type: text/html; charset=UTF-8\r\n\r\n")

def main():
	# Start the HTML response
	print("<!DOCTYPE html>")
	print("<html lang='en'>")
	print("<head>")
	print("<meta charset='UTF-8'>")
	print("<title>POST Request Test</title>")
	print("</head>")
	print("<body>")
	print("<h1>POST Request Test</h1>")

	# Create a form for testing
	print("<form method='POST' action='test_post.py'>")
	print("Enter some data: <input type='text' name='data' />")
	print("<input type='submit' value='Submit' />")
	print("</form>")

	# Check if the request method is POST
	body = sys.stdin.read()
	
	form = cgi.FieldStorage()
	if body:
		print("<h2>Received Data:</h2>")
		print("<p>" + html.escape(body) + "</p>")
	else:
		print("<p>No data received. Please submit the form.</p>")
	if os.environ.get("REQUEST_METHOD") == "POST":
		if form.getvalue('data'):
			# Read the posted data
			posted_data = form.getvalue('data')
			print("<h2>Received Data:</h2>")
			print("<p>" + html.escape(posted_data) + "</p>")
		else:
			print("<p>No data received. Please submit the form.</p>")
	else:
		print("<p>This is a GET request. Please submit the form.</p>")

	print("</body>")
	print("</html>")

if __name__ == "__main__":
	main()

