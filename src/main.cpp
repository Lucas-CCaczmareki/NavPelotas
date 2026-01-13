#include <iostream>
#include <vector>
#include <string>

int main() {
    std::cout << "Hello C++!\n";

    // int x;
    // float b;

    std::vector<int> v;

    for(int i = 0; i < 10; i++) {
        // std::cout << "Digite um numero: ";
        v.push_back(i);
    }

    // int var : container
    // a variavel recebe o próximo valor do container a cada iteração
    // equivale a percorrer o vetor do inicio ao fim
    for(int x : v) {
        std::cout << x;
        if (x < static_cast<int>(v.size()) - 1) {
            std::cout << ", ";
        }
    }
    
    // std::string linha;

    // std::cin >> linha;  //aqui lê até o espaço
    // std::cin.ignore();  // consome um caracter do buffer, nesse caso o espaço
    // std::cout << linha << "\n";

    // //aqui vai ler o resto do buffer "aranha\n" e vai printas
    // std::getline(std::cin, linha); //aqui lê até o \n
    // std::cout << linha;

    
    // std::cout << "Type an int, then type a float.\n";
    // std::cin >> a >> b;
    // std::cout << "You typed: " << a << ", " << b << "\n";

    // v.push_back(a);
    // v.push_back(b); //aqui é pra dar erro ou truncar

    // std::cout << "v[0]: " << v[0] << "\nv[1]: " << v[1] << "\n";


    return 0;
}