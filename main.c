#include <stdlib.h>
#include "data_source.h"
#include "parse_args.h"

int main(int argc, char *argv[])
{
    Filters filters = parse_args(argc, argv);
    init_data_source(filters.containers_path, filters.paths_path);

    if (filters.special_flag) {
        print_stations();
    } else {
        print_containers(filters);
    }

    destroy_data_source();
    return EXIT_SUCCESS;
}
