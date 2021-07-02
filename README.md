# tls-multi-echo-server
Implement a TLS 1.2 multi echo server using openssl

\n
<Compile>\n
gcc -o server echo_mpserv.c -lssl -lcrypto
gcc -o client echo_client.c -lssl -lcrypto

  /n
<Implement>
./server <port>
./client <ip> <port>
