#include <stdlib.h>
#include "data_source.h"

int main(int argc, char *argv[])
{
    Filters filters = parse_args(argc, argv);
    init_data_source(filters.containers_path, filters.paths_path);
    print_containers(filters);
    destroy_data_source();
    return EXIT_SUCCESS;
}
