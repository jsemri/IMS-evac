#include <iostream>

#include <unistd.h>
#include <getopt.h>

#include "evac_ca.h"

const char *helpstr =
"Program for simulating evacuation of building.\n"
"Usage: evac INPUT [OPTIONS] ...\n"
"  -h            : show this help and exit\n"
"  -t <DELAY>    : set delay of next step of evolution in ms, if not \n"
"                  specified the steps of the evolution are managed bye \n"
"                  hitting the Enter key\n"
"  -p <N>        : number of people to evacuate, default 100\n";

int main(int argc, char **argv) {

    long delay =  -1;   // simulation time
    int people = 100;   // persons to evacuate
    int opt_cnt = 1;    // used for locating positional argument
    int c;              // reading the options

    while ((c = getopt(argc, argv, "ht:p:")) != -1) {
        opt_cnt++;
        switch (c) {
            // TODO add more options later
            case 'h':
                fprintf(stderr, "%s", helpstr);
                return 0;
            case 't':
                opt_cnt++;
                delay = 1000 * std::stoi(optarg); // in microseconds
                break;
            case 'p':
                opt_cnt++;
                people = std::stoi(optarg);
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
    EvacCA ca = EvacCA::load_from_pixmap(argv[opt_cnt]);
    ca.add_people(people);

    // evolve CA in loop until CA can't change its states
    while (ca.evolve()) {
        // showing the current state of CA
        ca.show();
        if (delay != -1) {
            usleep(delay);
        }
        else {
            while(std::getchar() != '\n');
        }
    }

    // TODO some statistics, plots, etc.

    return 0;
}