#include "includes/RAM.hpp"
#include "includes/perifericos.hpp"
#include "includes/Scheduler.hpp"
#include "includes/Disco.hpp"
#include <vector>
#include <iostream>

using namespace std;

int main() {

    RAM ram;
    Disco disco;
    Perifericos periferico;
    vector<unique_ptr<Core>> cores;

    Scheduler scheduler(ram, disco, cores);

    cout << endl << endl << endl;

    ram.displayI();

    cout << endl << "Todos os processos foram finalizados." << endl;

    return 0;
}













