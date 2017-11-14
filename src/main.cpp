#include <iostream>

#include <unistd.h>
#include <getopt.h>

#include "evac_ca.h"

const char *helpstr =
"Program for simulating evacuation of building.\n"
"Usage: evac INPUT [OPTIONS] ...\n"
"  -h            : show this help and exit\n"
"  -t <DELAY>    : set delay of next step of evolution in ms, default 1s\n"
"  -p <N>        : number of people to evacuate, default 100\n";

int main(int argc, char **argv) {

    long delay = 1000 * 1000; // in microseconds
    int people = 100;
    int opt_cnt = 1;
    int c;

    while ((c = getopt(argc, argv, "hd:p:")) != -1) {
        opt_cnt++;
        switch (c) {
            // TODO add more options later
            case 'h':
                fprintf(stderr, "%s", helpstr);
                return 0;
            case 't':
                delay = 1000 * std::stoi(optarg);
                break;
            case 'p':
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

    EvacCA ca = EvacCA::load_from_pixmap(argv[opt_cnt]);
    ca.add_people(people);

    // evolve CA in loop until CA can't change its states
    while (ca.evolve()) {
        ca.show();
        usleep(delay);
    }

    return 0;
}
