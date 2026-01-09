#include <QCoreApplication>
#include <iostream>
#include <QDebug>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Set up code that uses the Qt event loop here.
    // Call a.quit() or a.exit() to quit the application.
    // A not very useful example would be including
    // #include <QTimer>
    // near the top of the file and calling
    // QTimer::singleShot(5000, &a, &QCoreApplication::quit);
    // which quits the application after 5 seconds.

    // If you do not need a running Qt event loop, remove the call
    // to a.exec() or use the Non-Qt Plain C++ Application template.
    // 1. Teste do Output do Qt (qDebug)
    qDebug() << "--------------------------------------------------";
    qDebug() << "Ola Mundo! O ambiente Qt esta configurado corretamente.";

    // 2. Teste do Output Padrão C++ (cout)
    std::cout << "Teste de std::cout: O compilador C++ esta funcionando." << std::endl;

    // 3. Teste de Instanciação (Verifica se o tradutor.h está sem erros)

    return a.exec();
}
