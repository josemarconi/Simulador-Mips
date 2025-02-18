#include "includes/RAM.hpp"
#include "includes/perifericos.hpp"
#include "includes/Scheduler.hpp"
#include "includes/Disco.hpp"
#include <vector>
#include <iostream>
#include "includes/Cache.hpp"

using namespace std;

int main() {

    RAM ram;
    Disco disco;
    Perifericos periferico;
    Cache cache(8);
    vector<unique_ptr<Core>> cores;

    Scheduler scheduler(ram, disco, cores, cache);

    cout << endl << "Todos os processos foram finalizados." << endl;

    return 0;
}













