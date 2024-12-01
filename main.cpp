include "ConsoleManager.h"


int main() {
    // init
    ConsoleManager::initialize();
    ConsoleManager* csm = ConsoleManager::get();

    // start
    csm->start();

    return 0;
}
