#!/usr/bin/env python3

import http.server
import socketserver
import subprocess
import os
import urllib.parse
import json
from pathlib import Path

class AttendanceHandler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        # Set the directory to serve static files from
        current_dir = os.path.dirname(os.path.abspath(__file__))
        frontend_dir = os.path.join(current_dir, "frontend")
        super().__init__(*args, directory=frontend_dir, **kwargs)
    
    def do_POST(self):
        if self.path.startswith('/cgi-bin/'):
            self.handle_cgi()
        else:
            self.send_error(404)
    
    def do_GET(self):
        if self.path.startswith('/cgi-bin/'):
            self.handle_cgi()
        else:
            # Serve static files
            super().do_GET()
    
    def handle_cgi(self):
        # Extract CGI script name
        path_parts = self.path.split('?')
        script_path = path_parts[0]
        query_string = path_parts[1] if len(path_parts) > 1 else ""
        
        # Map to actual CGI executable
        cgi_name = script_path.split('/')[-1]
        current_dir = os.path.dirname(os.path.abspath(__file__))
        cgi_executable = os.path.join(current_dir, "cgi-bin", cgi_name)
        
        if not os.path.exists(cgi_executable):
            self.send_error(404)
            return
        
        # Set up environment
        env = os.environ.copy()
        env['QUERY_STRING'] = query_string
        env['REQUEST_METHOD'] = self.command
        
        # Handle POST data
        post_data = b''
        if self.command == 'POST':
            content_length = int(self.headers.get('Content-Length', 0))
            post_data = self.rfile.read(content_length)
            env['CONTENT_LENGTH'] = str(content_length)
        
        try:
            # Change to the project directory so CGI can find data files
            original_cwd = os.getcwd()
            current_dir = os.path.dirname(os.path.abspath(__file__))
            os.chdir(current_dir)
            
            # Execute CGI script
            process = subprocess.Popen(
                [cgi_executable],
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                env=env
            )
            
            stdout, stderr = process.communicate(input=post_data)
            
            # Restore original directory
            os.chdir(original_cwd)
            
            if process.returncode == 0:
                # Parse CGI output (headers + body)
                output = stdout.decode('utf-8')
                if '\r\n\r\n' in output:
                    headers, body = output.split('\r\n\r\n', 1)
                else:
                    headers, body = '', output
                
                # Send response
                self.send_response(200)
                
                # Parse and send headers
                for header_line in headers.split('\r\n'):
                    if ':' in header_line:
                        header, value = header_line.split(':', 1)
                        self.send_header(header.strip(), value.strip())
                
                self.end_headers()
                self.wfile.write(body.encode('utf-8'))
            else:
                self.send_error(500, f"CGI Error: {stderr.decode('utf-8')}")
                
        except Exception as e:
            self.send_error(500, f"Server Error: {str(e)}")

if __name__ == "__main__":
    PORT = 8000
    
    print(f"Starting Attendance Management System server on port {PORT}")
    print(f"Open your browser and go to: http://localhost:{PORT}")
    print("Use the following credentials to test:")
    print("  Admin: admin / admin123")
    print("  Faculty: faculty / faculty123") 
    print("  Student: S001 / student123")
    print("\nPress Ctrl+C to stop the server")
    
    with socketserver.TCPServer(("", PORT), AttendanceHandler) as httpd:
        try:
            httpd.serve_forever()
        except KeyboardInterrupt:
            print("\nShutting down server...")
            httpd.shutdown()