#include <iostream>
#include <ctime>

#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <signal.h>

#include "evacuation.h"
#include "bitmap.h"    // remove after debug

const char *helpstr =
"Program for simulating evacuation of building.\n"
"Usage: evac INPUT [OPTIONS] ...\n"
"  -h            : show this help and exit\n"
"  -t <DELAY>    : set delay of next step of evolution in ms, default 400\n"
"  -p <N>        : number of people to evacuate, default 100\n"
"  -s <N>        : number of cells with smoke\n";

int main(int argc, char **argv) {
    long delay = 1000 * 400;   // simulation time
    int people = 100;   // persons to evacuate
    int opt_cnt = 1;    // used for locating positional argument
    int smoke = -1;     // cells with smoke
    int c;              // reading the options

    while ((c = getopt(argc, argv, "ht:p:s:")) != -1) {
        opt_cnt += 2;
        switch (c) {
            case 'h':
                fprintf(stderr, "%s", helpstr);
                return 0;
            case 't':
                delay = 1000 * std::stoi(optarg); // in milliseconds
                break;
            case 'p':
                people = std::stoi(optarg);
                break;
            case 's':
                smoke = std::stoi(optarg);
                break;
            default:
                return 1;
        }
    }
    // check for positional arguments
    if (argc - opt_cnt != 1) {
        std::cerr << "Error: invalid arguments\n";
        return 1;
    }

    // reading a pix
    Evacuation::CA ca = Evacuation::CA::load(argv[opt_cnt]);
    ca.add_people(people);
    ca.add_smoke(smoke);
    // uncoment for displaying heat map
    Bitmap::display_distances(ca);
    // uncoment this for opening image with xdg-open
    if (delay > 0)
        std::system("xdg-open output.bmp");

    std::srand(std::time(0));
    // evolve CA in loop until CA can't change its states
    while (ca.evolve()) {
        // showing the current state of CA
        if (delay > 0)
            ca.show();
        usleep(delay);
    }
    ca.show();
    ca.print_statistics();

    return 0;
}
