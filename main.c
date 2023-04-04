#include <stdlib.h>
#include "data_source.h"

int main(int argc, char *argv[])
{
    init_data_source(argv[1], argv[2]);
    print_containers();
    destroy_data_source();
    return EXIT_SUCCESS;
}
