#docker run --rm --cap-add=NET_ADMIN --network cpp-tcpip --name my-server my-server
docker run -it --cap-add=NET_ADMIN --name server --network my_network --ip 192.168.1.2 my-server /bin/bash
