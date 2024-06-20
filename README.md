## Non-Blocking Web Server in C++

This project implements a small web server in C++ that supports multiple concurrent connections without using multi-threading or blocking sockets. The server is designed to handle various HTTP methods.

### Features

* **HTTP Methods Supported:** OPTIONS, GET, HEAD, POST, PUT, DELETE, TRACE
* **Language Support for GET Requests:**
    * Query string parameter `lang` can be used to request the page in different languages:
        * `lang=he` for Hebrew
        * `lang=en` for English
        * `lang=fr` for French
* **POST Requests:** The server logs the received strings to the console.
* **Non-Blocking Sockets:** Handles multiple connections concurrently using non-blocking sockets.
* **Timeout Management:** Closes connections if the request is idle for more than two minutes.
* **Content-Type:** Supports text/html responses.

### Installation

1. **Clone the repository:**

   ```bash
   git clone https://github.com/Ronenii/HTTP-TCP-Server.git
   ```

2. **Navigate to the project directory:**

   ```bash
   cd HTTP-TCP-Server
   ```

3. **Compile the server:** (Replace `make` with your build command if needed)

   ```bash
   make
   ```

4. **Add test files:**

Add the contents of the /pages directory to the folder C:\temp.


### Usage

1. **Start the server:**

   ```bash
   ./server
   ```

2. **Access the server:**

   * Using a web browser: http://localhost:8080
   * Using an HTTP client (e.g., curl, Postman)

3. **Test different HTTP methods:**

   * GET request (specifying language):
     ```bash
     curl -X GET "http://localhost:8080/?lang=en"
     ```
   * POST request:
     ```bash
     curl -X POST -d "data=example" http://localhost:8080
     ```

### Development

#### Server Implementation

The server is implemented using non-blocking sockets and the `select` system call to manage multiple concurrent connections. It supports essential HTTP methods and language-based responses for GET requests.

#### Handling GET Requests

When a GET request is received, the server checks for the `lang` query parameter and returns the appropriate language version of the page if available. If no language is specified or the requested language is not supported, the server defaults to English.

#### Handling POST Requests

POST requests are expected to contain strings which the server logs to the console for demonstration purposes.

### Testing

#### Tools Used

* Wireshark: For capturing and analyzing network traffic.
* cURL: For sending various HTTP requests to the server.

#### Test Procedure

1. Use Wireshark to capture HTTP traffic to and from the server.
2. Send different types of HTTP requests (OPTIONS, GET, HEAD, POST, PUT, DELETE, TRACE) and verify the responses.
3. Check the server console for POST request data logging.
4. Validate that connections are properly closed after being idle for more than two minutes.
