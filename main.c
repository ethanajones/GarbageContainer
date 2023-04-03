#include <stdlib.h>
#include "data_source.h"
#include "container.h"

// TODO: [Optional] Think about an appropriate division of the solution into files.
//                  e.g. Move data loading and validating to another .c file.

// TODO: Read the content of this file.

int main(int argc, char *argv[])
{
    // TODO: Read and validate options.

    // This is only a demo, you should probably move the following line somewhere else.

    init_data_source(argv[1], argv[2]);
    print_containers();


    // TODO: Save data to your own structures.

    destroy_data_source(); // If you forget to do this, Valgrind will yell at you a lot.

    // TODO: Process the data as the user wishes and print them to the stdin.

    return EXIT_SUCCESS; // May your program be as successful as this macro. Good luck!
}
