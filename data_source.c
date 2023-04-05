#include "data_source.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include "container.h"

// Container CSV column header
#define CONTAINER_COLUMNS_COUNT 9

#define CONTAINER_ID 0
#define CONTAINER_X 1
#define CONTAINER_Y 2
#define CONTAINER_WASTE_TYPE 3
#define CONTAINER_CAPACITY 4
#define CONTAINER_NAME 5
#define CONTAINER_STREET 6
#define CONTAINER_NUMBER 7
#define CONTAINER_PUBLIC 8

// Path CSV column header
#define PATH_COLUMNS_COUNT 3

#define PATH_A 0
#define PATH_B 1
#define PATH_DISTANCE 2

struct data_source {
    char ***containers;
    size_t containers_count;

    char ***paths;
    size_t paths_count;
};

typedef struct {
    const char *id;
    double distance;
} Neighbor;


static struct data_source *data_source;

static char *readline(FILE *file) {
    assert(file != NULL);

    size_t line_size = 32;
    size_t offset = 0;
    char *line = NULL;
    size_t add = 32;

    char *tmp;
    do {
        tmp = realloc(line, line_size + 1);
        if (tmp == NULL) {
            free(line);
            return NULL;
        }

        line = tmp;
        memset(line + (line_size - add), 0, add);

        if (fgets(line + offset, add + 1, file) == NULL) {
            break;
        }

        offset = line_size;
        line_size += add;

    } while (strchr(line, '\n') == NULL);

    if (ferror(file) || feof(file)) {
        free(line);
        return NULL;
    }

    line[strlen(line) - 1] = '\0';

    return line;
}

static void free_pointer_array(void **array, size_t size) {
    assert(array != NULL);

    for (size_t index = 0; index < size; index++) {
        free(array[index]);
    }

    free(array);
}

static void free_splitted_lines(char ***lines, int column_count) {
    assert(lines != NULL);

    for (int index = 0; lines[index] != NULL; index++) {
        free_pointer_array((void **) lines[index], column_count);
    }

    free(lines);
}

static char **split_csv_line(char *line, size_t expected_count) {
    assert(line != NULL);

    char **splitted_line = calloc(expected_count + 1, sizeof(char *));
    if (splitted_line == NULL) {
        return NULL;
    }
    size_t line_length = strlen(line);

    char *token = strtok(line, ",");
    size_t parsed_length = 0;

    for (size_t index = 0; index < expected_count; index++) {
        if (token == NULL) {
            free_pointer_array((void **) splitted_line, index);
            return NULL;
        }

        parsed_length += strlen(token) + 1;

        splitted_line[index] = malloc(strlen(token) + 1);
        if (splitted_line[index] == NULL) {
            free_pointer_array((void **) splitted_line, index);
            return NULL;
        }

        strcpy(splitted_line[index], token);

        if (line_length > parsed_length
            && strchr(line + parsed_length, ',') == line + parsed_length) {
            token = "";
        } else {
            token = strtok(NULL, ",");
        }
    }

    if (token != NULL) {
        free_pointer_array((void **) splitted_line, expected_count);
        return NULL;
    }

    return splitted_line;
}

static char ***parse_csv(const char *path, int column_count) {
    assert(path != NULL);

    FILE *csv_file = fopen(path, "r");

    if (csv_file == NULL) {
        return NULL;
    }

    size_t array_capacity = 8;
    size_t line_count = 0;
    char ***lines = calloc(array_capacity + 1, sizeof(char **));

    if (lines == NULL) {
        fclose(csv_file);
        return NULL;
    }

    char *line;
    void *tmp;

    while ((line = readline(csv_file)) != NULL) {
        if (line_count >= array_capacity) {
            tmp = realloc(lines, (array_capacity * 2 + 1) * sizeof(char **));
            if (tmp == NULL) {
                free(line);
                free_splitted_lines(lines, column_count);
                fclose(csv_file);
                return NULL;
            }
            lines = tmp;
            memset(lines + line_count, 0, (array_capacity + 1) * sizeof(char **));
            array_capacity *= 2;
        }

        if ((lines[line_count] = split_csv_line(line, column_count)) == NULL) {
            free(line);
            free_splitted_lines(lines, column_count);
            fclose(csv_file);
            return NULL;
        }

        line_count++;
        free(line);
    }

    if (ferror(csv_file)) {
        free_splitted_lines(lines, column_count);
        fclose(csv_file);
        return NULL;
    }

    fclose(csv_file);
    return lines;
}

static size_t count_lines(void **lines) {
    size_t lines_count = 0;
    while (lines[lines_count] != NULL) {
        lines_count++;
    }
    return lines_count;
}

bool init_data_source(const char *containers_path, const char *paths_path) {
    data_source = malloc(sizeof(struct data_source));
    if (data_source == NULL) {
        return false;
    }
    data_source->containers = parse_csv(containers_path, CONTAINER_COLUMNS_COUNT);
    if (data_source->containers == NULL) {
        free(data_source);
        return false;
    }
    data_source->containers_count = count_lines((void **) data_source->containers);

    data_source->paths = parse_csv(paths_path, PATH_COLUMNS_COUNT);
    if (data_source->paths == NULL) {
        free_splitted_lines(data_source->containers, CONTAINER_COLUMNS_COUNT);
        free(data_source);
        return false;
    }
    data_source->paths_count = count_lines((void **) data_source->paths);

    return true;
}

void destroy_data_source(void) {
    free_splitted_lines(data_source->containers, CONTAINER_COLUMNS_COUNT);
    free_splitted_lines(data_source->paths, PATH_COLUMNS_COUNT);
    free(data_source);
}

const char *get_container_id(size_t line_index) {
    if (line_index >= data_source->containers_count) {
        return NULL;
    }
    return data_source->containers[line_index][CONTAINER_ID];
}

const char *get_container_x(size_t line_index) {
    if (line_index >= data_source->containers_count) {
        return NULL;
    }
    return data_source->containers[line_index][CONTAINER_X];
}

const char *get_container_y(size_t line_index) {
    if (line_index >= data_source->containers_count) {
        return NULL;
    }
    return data_source->containers[line_index][CONTAINER_Y];
}

const char *get_container_waste_type(size_t line_index) {
    if (line_index >= data_source->containers_count) {
        return NULL;
    }
    return data_source->containers[line_index][CONTAINER_WASTE_TYPE];
}

const char *get_container_capacity(size_t line_index) {
    if (line_index >= data_source->containers_count) {
        return NULL;
    }
    return data_source->containers[line_index][CONTAINER_CAPACITY];
}

const char *get_container_name(size_t line_index) {
    if (line_index >= data_source->containers_count) {
        return NULL;
    }
    return data_source->containers[line_index][CONTAINER_NAME];
}

const char *get_container_street(size_t line_index) {
    if (line_index >= data_source->containers_count) {
        return NULL;
    }
    return data_source->containers[line_index][CONTAINER_STREET];
}

const char *get_container_number(size_t line_index) {
    if (line_index >= data_source->containers_count) {
        return NULL;
    }
    return data_source->containers[line_index][CONTAINER_NUMBER];
}

const char *get_container_public(size_t line_index) {
    if (line_index >= data_source->containers_count) {
        return NULL;
    }
    return data_source->containers[line_index][CONTAINER_PUBLIC];
}

const char *get_path_a_id(size_t line_index) {
    if (line_index >= data_source->paths_count) {
        return NULL;
    }
    return data_source->paths[line_index][PATH_A];
}

const char *get_path_b_id(size_t line_index) {
    if (line_index >= data_source->paths_count) {
        return NULL;
    }
    return data_source->paths[line_index][PATH_B];
}

const char *get_path_distance(size_t line_index) {
    if (line_index >= data_source->paths_count) {
        return NULL;
    }
    return data_source->paths[line_index][PATH_DISTANCE];
}

Filters parse_args(int argc, char *argv[]) {
    Filters filters = {{"", "", "", "", "", "", "", ""}, 0, 0, 0, NULL, NULL};
    int opt;

    while ((opt = getopt(argc, argv, "t:c:p:")) != -1) {
        switch (opt) {
            case 't':
                for (size_t i = 0; optarg[i] != '\0' && filters.waste_type_count < 8; ++i) {
                    filters.waste_types[filters.waste_type_count][0] = optarg[i];
                    filters.waste_types[filters.waste_type_count][1] = '\0';
                    filters.waste_type_count++;
                }
                break;
            case 'c':
                sscanf(optarg, "%d-%d", &filters.capacity_min, &filters.capacity_max);
                break;
            case 'p':
                // Modify support for public filter
                if (optarg[0] == 'N') {
                    filters.public_filter = 1;
                } else if (optarg[0] == 'Y') {
                    filters.public_filter = -1;
                } else {
                    fprintf(stderr, "Invalid value for public_filter. Use 'Y' or 'N'.\n");
                    exit(EXIT_FAILURE);
                }
                break;
            default:
                fprintf(stderr,
                        "Usage: %s [-t waste_type] [-c min_capacity-max_capacity] [-p public_filter] containers_file paths_file\n",
                        argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (optind + 1 >= argc) {
        fprintf(stderr, "Expected containers_file and paths_file arguments\n");
        exit(EXIT_FAILURE);
    }

    filters.containers_path = argv[optind];
    filters.paths_path = argv[optind + 1];

    return filters;
}


Neighbor *find_neighbors(const char *given_container_id, size_t *neighbors_count) {
    Neighbor *neighbors = NULL;
    *neighbors_count = 0;

    for (size_t i = 0; i < data_source->paths_count; i++) {
        const char *container_a_id = get_path_a_id(i);
        const char *container_b_id = get_path_b_id(i);

        if (strcmp(given_container_id, container_a_id) == 0 ||
            strcmp(given_container_id, container_b_id) == 0) {
            const char *neighbor_id = strcmp(given_container_id, container_a_id) == 0 ? container_b_id : container_a_id;
            double distance = strtod(get_path_distance(i), NULL);

            neighbors = realloc(neighbors, (*neighbors_count + 1) * sizeof(Neighbor));
            neighbors[*neighbors_count].id = neighbor_id;
            neighbors[*neighbors_count].distance = distance;
            (*neighbors_count)++;
        }
    }

    return neighbors; // Caller should free the memory allocated for neighbors
}

void print_containers(Filters filters) {
    for (int i = 0; i < data_source->containers_count; i++) {
        const char *type = data_source->containers[i][CONTAINER_WASTE_TYPE];
        int capacity = atoi(data_source->containers[i][CONTAINER_CAPACITY]);
        int public_value = atoi(data_source->containers[i][CONTAINER_PUBLIC]);

        bool waste_type_match = false;
        if (filters.waste_types[0] == '\0') {
            waste_type_match = true;
        } else {
            for (size_t j = 0; j < filters.waste_type_count; j++) {
                char waste_type = filters.waste_types[j][0]; // Access the first character of the waste type string
                switch (waste_type) {
                    case 'A':
                        waste_type_match |= (strcmp(type, "Plastics and Aluminium") == 0);
                        break;
                    case 'P':
                        waste_type_match |= (strcmp(type, "Paper") == 0);
                        break;
                    case 'B':
                        waste_type_match |= (strcmp(type, "Biodegradable waste") == 0);
                        break;
                    case 'G':
                        waste_type_match |= (strcmp(type, "Clear glass") == 0);
                        break;
                    case 'C':
                        waste_type_match |= (strcmp(type, "Colored glass") == 0);
                        break;
                    case 'T':
                        waste_type_match |= (strcmp(type, "Textile") == 0);
                        break;
                }
                if (waste_type_match) {
                    break;
                }
            }
        }

        bool capacity_match = ((filters.capacity_min == 0 && filters.capacity_max == 0) ||
                               (capacity >= filters.capacity_min && capacity <= filters.capacity_max));

        bool public_match = (filters.public_filter == -1 || public_value == filters.public_filter);

        if (waste_type_match && capacity_match && public_match) {
            printf("ID: ");
            printf(data_source->containers[i][0]); // ID
            printf(", ");
            printf("Type: ");
            printf(data_source->containers[i][3]); // Type
            printf(", ");
            printf("Capacity: ");
            printf(data_source->containers[i][4]); // Container Capacity
            printf(", ");
            printf("Address: ");
            printf(data_source->containers[i][6]); // Street
            printf("Neighbors: ");
            size_t neighbors_count;
            Neighbor *neighbors = find_neighbors(data_source->containers[i][0], &neighbors_count);
            for (size_t j = 0; j < neighbors_count; j++) {
                printf("%s", neighbors[j].id);
                if (j < neighbors_count - 1) {
                    printf(" ");
                }
            }
            free(neighbors);
            printf("\n");
        }
    }
}
