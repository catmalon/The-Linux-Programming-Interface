# Part 1

Set up a client and server while talk over TCP AF_UNIX sockets. The server performs the dictionary look up.
1. Edit the lookup7.c file to communicate with a server by using TCP AF_UNIX sockets.
2. Edit usock_server.c to listen on an AF_UNIX socket for any number of TCP AF_UNIX clients and reply down the same socket.
3. After the file have been edited, type make, or make usock_server and usock_cilent.
4. When you get the prompt, run the usock_server and usock_client as shown in the output below.

![螢幕擷取畫面 2021-12-22 182245](https://user-images.githubusercontent.com/75157669/147078338-ec142b0f-fd90-4200-9048-7ec6802e3579.png)

#### Server 
 Usage: ./usock_server <dictionary source> <Socket name>

 ![螢幕擷取畫面 2021-12-22 181445](https://user-images.githubusercontent.com/75157669/147078755-420ab677-3c38-4d30-a33f-1c26160e82fb.png)

#### Client
  Usage: ./usock_client <resource>

  ![image](https://user-images.githubusercontent.com/75157669/147078847-9aff8ba0-756f-4b0e-9aaf-a2724e027785.png)

# Part 2
Set up a client and server while talk over TCP AF_INET sockets. The server performs the dictionary look up.
1. Edit the lookup8.c file to communicate with a server by using TCP AF_INET sockets.
2. Edit isock_server.c to listen on an TCP AF_INET socket for any number of internet clients and reply down the same socket.
3. After the file have been edited, type make, or make isock_server and isock_cilent.
4. When you get the prompt, run the isock_server and isock_client.
  
![image](https://user-images.githubusercontent.com/75157669/147078962-c25ff17f-602d-45d5-9400-678dbdc57c2f.png)

#### Server 
 Usage: ./isock_server <dictionary source>
  
![image](https://user-images.githubusercontent.com/75157669/147079249-a9a72d4d-d3f7-4465-8d8e-9dd28e426207.png)

#### Client
  Usage: ./isock_client <resource>
  
  ![image](https://user-images.githubusercontent.com/75157669/147079508-7fcf1d6b-2029-4998-9348-7fac875538ce.png)
  
# Part 3
Set up a client and server while talk over UDP AF_INET sockets. server performs the dictionary look up.
1. Edit the lookup9.c file to communicate with a server by using UDP AF_INET sockets.
2. Edit udp_server.c to listen on an UDP AF_INET socket for any number of internet clients and reply down the same socket.
3. After the file have been edited, type make, or make udp_server and udp_cilent.
4. When you get the prompt, run the udp_server and udp_client as shown in the output below.
  
  ![image](https://user-images.githubusercontent.com/75157669/147079772-74fe871f-f055-4421-9d48-b10b808e4cd8.png)

  
#### Server 
 Usage: ./udp_server <dictionary source>
  
![image](https://user-images.githubusercontent.com/75157669/147079905-fb6e76d7-c135-4878-90fd-70ed30aeae78.png)

#### Client
  Usage: ./udp_client <resource>
  
![image](https://user-images.githubusercontent.com/75157669/147080295-2ff2e289-9831-437e-b3aa-f262b558b339.png)
