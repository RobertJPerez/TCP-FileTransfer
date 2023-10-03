Robert Perez - Programing Assignment 1

Running Instructions (using VScode) - 

1. Open integrated terminal on folder containing files
2. If server.exe/client.exe do not exist run in the terminal: "gcc client.c -o client"
3. Next run "gcc -o server server.c" 
4. Next in the command line type: ./server 
5. Next run ./client
6. Done 

Files: 

server.c 
client.c 
README.txt

Reflection and Result: 

When running my files I get packet 0 recieved repeating. I think the root of this issue is in my server.c file 
and i tried to increment the packet_seq_num variable before sending the packet header amongst other things but they do not work.
I tried many fixes and got some of the other messages to appear such as "End of transmission" but this would break the packet totals. If I modify this function
I am able to get the packet transfered message. Looking back I tried my best to implement everything as best as possible and proud for the progress I made. I know my client.c is a few 
lines away from this project working perfectly! This project taught various things about implementing a client/server relationships most specifically
(and most challengingly) the send file function. This function required me to work with sockets to read data of specific sizes. Each iteration creates a new Packetheader
that is used to send the data. This function was the hardest but taught me the most. Client.c was more approachable as it was a reflection of server but I think
working back and forth between files is where I ran into my issue. 

Overall this project was a informative intro into socket programing! 
