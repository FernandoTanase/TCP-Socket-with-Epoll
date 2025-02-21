# TCP-Socket-with-Epoll
Implementation of a client-server application in C using TCP sockets. The server uses a single-threaded, event-driven model with Epoll to handle multiple client connections, while the client uses multiple threads to establish connections and communicate with the server. The client also measures round-trip-time (RTT) and request rates.
