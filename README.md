# IPC-Messages
Coded C programs that uses message queues between server and clients to play Hangman. When a request is received the server creates a new child process to handle the request, and sends its PID to the client. Thus allowing the child process to receive messages from client containing the guess for the word and send updated information of the game.
