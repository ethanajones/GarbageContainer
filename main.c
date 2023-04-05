#include <stdlib.h>
#include "data_source.h"

int main(int argc, char *argv[])
{
    //Task 2
    Filters filters = parse_args(argc, argv);
    //Task 1
    init_data_source(filters.containers_path, filters.paths_path);
    print_containers(filters);
    destroy_data_source();
    return EXIT_SUCCESS;
}
