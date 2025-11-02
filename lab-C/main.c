#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

struct node {
    int vertex;
    struct node* next;
};

struct graph {
    int num_vertices;
    struct node** adj_lists;
    bool* visited;
};

struct node* create_node(int v) {
    struct node* new_node = malloc(sizeof(struct node));
    new_node->vertex = v;
    new_node->next = NULL;
    return new_node;
}

struct graph* create_graph(int vertices) {
    struct graph* graph = malloc(sizeof(struct graph));
    graph->num_vertices = vertices;

    graph->adj_lists = malloc(vertices * sizeof(struct node*));
    graph->visited = malloc(vertices * sizeof(bool));

    for (int i = 0; i < vertices; i++) {
        graph->adj_lists[i] = NULL;
        graph->visited[i] = false;
    }

    return graph;
}

void add_edge(struct graph* graph, int src, int dest) {
    struct node* new_node = create_node(dest);
    new_node->next = graph->adj_lists[src];
    graph->adj_lists[src] = new_node;
}

struct graph* read_graph_from_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: cannot open file %s\n", filename);
        return NULL;
    }

    int num_vertices;
    if (fscanf(file, "%d", &num_vertices) != 1) {
        printf("Error: invalid file format\n");
        fclose(file);
        return NULL;
    }

    char buffer[256];
    fgets(buffer, sizeof(buffer), file);

    struct graph* graph = create_graph(num_vertices);

    for (int i = 0; i < num_vertices; i++) {
        if (fgets(buffer, sizeof(buffer), file) == NULL) {
            break;
        }

        char* colon_pos = strchr(buffer, ':');
        if (colon_pos == NULL) {
            continue;
        }

        int vertex = atoi(buffer);

        char* neighbors_str = colon_pos + 1;
        char* token = strtok(neighbors_str, ", \n");

        while (token != NULL) {
            int neighbor = atoi(token);
            if (neighbor >= 0 && neighbor < num_vertices) {
                add_edge(graph, vertex, neighbor);
            }
            token = strtok(NULL, ", \n");
        }
    }

    fclose(file);
    return graph;
}

bool dfs(struct graph* graph, int current, int target) {
    graph->visited[current] = true;

    if (current == target) {
        return true;
    }

    struct node* temp = graph->adj_lists[current];
    while (temp != NULL) {
        int adjacent_vertex = temp->vertex;
        if (!graph->visited[adjacent_vertex]) {
            if (dfs(graph, adjacent_vertex, target)) {
                return true;
            }
        }
        temp = temp->next;
    }

    return false;
}

bool path_exists(struct graph* graph, int start, int end) {
    for (int i = 0; i < graph->num_vertices; i++) {
        graph->visited[i] = false;
    }

    bool result = dfs(graph, start, end);

    for (int i = 0; i < graph->num_vertices; i++) {
        graph->visited[i] = false;
    }

    return result;
}

void free_graph(struct graph* graph) {
    for (int i = 0; i < graph->num_vertices; i++) {
        struct node* temp = graph->adj_lists[i];
        while (temp != NULL) {
            struct node* to_delete = temp;
            temp = temp->next;
            free(to_delete);
        }
    }
    free(graph->adj_lists);
    free(graph->visited);
    free(graph);
}

int original_main() {
    struct graph* graph = read_graph_from_file("text.txt");

    if (graph == NULL) {
        printf("Failed to load graph from file\n");
        return 1;
    }

    int start, end;
    char choice;

    do {
        printf("Enter start vertex (0-%d): ", graph->num_vertices - 1);
        scanf("%d", &start);
        printf("Enter end vertex (0-%d): ", graph->num_vertices - 1);
        scanf("%d", &end);

        if (start < 0 || start >= graph->num_vertices ||
            end < 0 || end >= graph->num_vertices) {
            printf("Error: vertices must be between 0 and %d\n", graph->num_vertices - 1);
        }
        else {
            printf("Path from %d to %d: %s\n\n", start, end,
                path_exists(graph, start, end) ? "exists" : "does not exist");
        }

        printf("Check another path? (y/n): ");
        scanf(" %c", &choice);
        printf("\n");

    } while (choice == 'y' || choice == 'Y');

    free_graph(graph);
    return 0;
}