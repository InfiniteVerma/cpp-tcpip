#docker run --rm --cap-add=NET_ADMIN --network cpp-tcpip --name my-client my-client
docker run --rm --cap-add=NET_ADMIN --name client --network my_network --ip 192.168.1.3 my-client
