FROM ubuntu:22.04

# Install build tools and valgrind
RUN apt-get update && \
  apt-get install -y build-essential valgrind

# Create working directory
WORKDIR /app

# Copy your source file into the container
COPY src ./src
COPY fileModule ./fileModule
COPY include ./include
COPY input.txt .


# Compile it
RUN g++ -std=c++17 -O2 src/main.cpp src/Graph.cpp src/dijkstra.cpp src/trie.cpp fileModule/fileModule.cpp -Iinclude -o main

# Default command runs Valgrind
CMD ["sh", "-c", "valgrind --leak-check=full --show-leak-kinds=all ./main < input.txt"]

