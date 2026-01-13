## Running with clang

- clang main.cpp -O0 -o main trie/trie.cpp fileModule/fileModule.cpp -lstdc++ && ./main && rm -rf ./main

## To debug run with:
- `clang++ -O0 -g main.cpp trie/trie.cpp fileModule/fileModule.cpp -o main`
- `lldb ./main`
- Inside lldb: `help` -- check commnands
- Short tutorial: https://dev.to/godinhojoao/introduction-to-gnu-gdb-elf-and-lldb-p9p