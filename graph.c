#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"

static int LINK_ID = 0;
static void __eulerian_check(Graph *graph);
static GraphLink __add_graph_link(Graph *graph, GraphNode *node, GraphNode *next_node, int link_id, const int value);
static void __remove_graph_link(Graph *graph, GraphNode *node, const int link_id);

// ---------------------------------------------

Graph create_graph(Bool oriented) {
    Graph graph = malloc(sizeof(GraphElement));

    if(graph == NULL) {
        fprintf(stderr, "Malloc");
        exit(EXIT_FAILURE);
    }

    graph->number_nodes = 0;
    graph->number_odd_nodes = 0;
    graph->nodes = NULL;
    graph->oriented = oriented;
    graph->related = false;
    graph->eulerian_string = false;
    graph->eulerian_cycle = false;

    return graph;
}

Bool is_empty_graph(Graph graph) {
    return graph->nodes == NULL;
}

void print_graph(Graph graph) {
    if(is_empty_graph(graph)) {
        printf("Empty graph");
        return;
    }

    printf("\n--------  Graph Info  --------\n");

    printf("\nOriented: %s\n", (graph->oriented) ? "yes" : "no");
    printf("Contain eulerian string: %s\n", (graph->eulerian_string) ? "yes" : "no");
    printf("Contain eulerian cycle: %s\n", (graph->eulerian_cycle) ? "yes" : "no");
    printf("Number of nodes: %d\n", graph->number_nodes);
    printf("Number of odd nodes: %d\n", graph->number_odd_nodes);

    printf("\n------------------------------\n\n");

    for (int i = 0; i < graph->number_nodes; ++i) {
        GraphNode node = graph->nodes[i];

        printf("[node: %s] -> ", node->name);

        if(node->number_links == 0) {
            printf("No link");
        } else {
            for (int j = 0; j < node->number_links; ++j) {
                GraphLink link = node->links[j];

                printf("(%s #%d)", link->next_node->name, link->value);

                if(j < node->number_links - 1)
                    printf(" | ");
            }
        }
        printf("\n");
    }

    printf("\n------  End Graph Info  ------\n\n");
}

void clear_graph(Graph *graph) {
    if(!is_empty_graph(*graph)) {
        while((*graph)->number_nodes != 0)
            remove_graph_node(&*graph, &(*graph)->nodes[0]);
    }
}

void destroy_graph(Graph *graph) {
    clear_graph(graph);

    free(*graph);
    graph = NULL;
}

static void __eulerian_check(Graph *graph) {
    int odd_nodes = (*graph)->number_odd_nodes;
    (*graph)->eulerian_string = (odd_nodes == 0 || odd_nodes == 2) ? true : false;
    (*graph)->eulerian_cycle = (odd_nodes == 0) ? true : false;
}

// ---------------------------------------------

GraphNode add_graph_node(Graph *graph, const char* name) {
    GraphNode node = malloc(sizeof(GraphNodeElement));

    if(node == NULL) {
        fprintf(stderr, "Malloc");
        exit(EXIT_FAILURE);
    }

    node->name = malloc(sizeof(char) * strlen(name) + 1);

    if(node->name == NULL) {
        fprintf(stderr, "Malloc");
        exit(EXIT_FAILURE);
    }

    memcpy(node->name, name, strlen(name) + 1);

    node->links = NULL;
    node->number_links = 0;

    if(is_empty_graph(*graph)) {
        (*graph)->nodes = malloc(sizeof(GraphNodeElement));

        if((*graph)->nodes == NULL) {
            fprintf(stderr, "Malloc");
            exit(EXIT_FAILURE);
        }

        (*graph)->nodes[0] = node;
        (*graph)->number_nodes++;

        return node;
    }

    (*graph)->nodes = (GraphNode*)realloc((*graph)->nodes, sizeof(GraphNodeElement) * ((*graph)->number_nodes + 1));

    if((*graph)->nodes == NULL) {
        fprintf(stderr, "Malloc");
        exit(EXIT_FAILURE);
    }

    (*graph)->nodes[(*graph)->number_nodes] = node;
    (*graph)->number_nodes++;

    return node;
}

Bool is_empty_graph_node(GraphNode node) {
    return node->links == NULL;
}

void remove_graph_node(Graph *graph, GraphNode *node) {
    while(!is_empty_graph_node(*node))
        __remove_graph_link(graph, node, (*node)->links[0]->link_id);

    int index = 0;
    for (int i = 0; i < (*graph)->number_nodes; i++) {
        if((*graph)->nodes[i] != *node) {
            (*graph)->nodes[index] = (*graph)->nodes[i];
            index++;
        } else {
            free((*node)->name);
            (*node)->name = NULL;

            free(*node);
            (*node) = NULL;
        }
    }

    if((*graph)->number_nodes > 1) {
        (*graph)->nodes = realloc((*graph)->nodes, sizeof(GraphNodeElement) * (*graph)->number_nodes - 1);

        if((*graph)->nodes == NULL) {
            fprintf(stderr, "Malloc");
            exit(EXIT_FAILURE);
        }
    } else {
        free((*graph)->nodes);
        (*graph)->nodes = NULL;
    }

    (*graph)->number_nodes--;
}

// ---------------------------------------------

GraphLink add_graph_link(Graph *graph, GraphNode *node, GraphNode *next_node, const int value) {
    for (int i = 0; i < (*next_node)->number_links; ++i) {
        if((*next_node)->links[i]->next_node == *node) {
            if((*graph)->oriented) {
                fprintf(stderr, "OrientedGraphError: [%s] -> [%s] : Already defined link", (*node)->name, (*next_node)->name);
                exit(EXIT_FAILURE);
            } else {
                remove_graph_link(graph, next_node, &(*next_node)->links[i]);
            }

            break;
        }
    }

    int link_id = ++LINK_ID;

    if(!(*graph)->oriented)
        __add_graph_link(graph, next_node, node, link_id, value);

    GraphLink link = __add_graph_link(graph, node, next_node, link_id, value);

    __eulerian_check(graph);

    return link;
}

static GraphLink __add_graph_link(Graph *graph, GraphNode *node, GraphNode *next_node, int link_id, const int value) {
    GraphLink link = malloc(sizeof(GraphLinkElement));

    if(link == NULL) {
        fprintf(stderr, "Malloc");
        exit(EXIT_FAILURE);
    }

    link->value = value;
    link->link_id = link_id;
    link->next_node = *next_node;

    (*graph)->number_odd_nodes += (((*node)->number_links + 1) % 2 == 0) ? -1 : 1;

    if(is_empty_graph_node((*node))) {
        (*node)->links = malloc(sizeof(GraphLinkElement));

        if((*node)->links == NULL) {
            fprintf(stderr, "Malloc");
            exit(EXIT_FAILURE);
        }

        (*node)->links[0] = link;
        (*node)->number_links++;
        return link;
    }

    (*node)->links = realloc((*node)->links, sizeof(GraphLinkElement) * (*node)->number_links + 1);

    if((*node)->links == NULL) {
        fprintf(stderr, "Malloc");
        exit(EXIT_FAILURE);
    }

    (*node)->links[(*node)->number_links] = link;
    (*node)->number_links++;

    return link;
}

void remove_graph_link(Graph *graph, GraphNode *node, GraphLink *link) {
    if(!(*graph)->oriented)
        __remove_graph_link(graph, &(*link)->next_node, (*link)->link_id);

    __remove_graph_link(graph, node, (*link)->link_id);

    __eulerian_check(graph);
}

static void __remove_graph_link(Graph *graph, GraphNode *node, const int link_id) {
    if(is_empty_graph_node(*node)) return;

    int index = 0;
    for (int i = 0; i < (*node)->number_links; i++) {
        if((*node)->links[i]->link_id != link_id) {
            (*node)->links[index] = (*node)->links[i];
            index++;
        } else {
            free((*node)->links[i]);
            (*node)->links[i] = NULL;
        }
    }

    (*graph)->number_odd_nodes += (((*node)->number_links - 1) % 2 == 0) ? -1 : 1;

    if((*node)->number_links > 1) {
        (*node)->links = realloc((*node)->links, sizeof(GraphLinkElement) * (*node)->number_links - 1);

        if((*node)->links == NULL) {
            fprintf(stderr, "Malloc");
            exit(EXIT_FAILURE);
        }
    } else {
        free((*node)->links);
        (*node)->links = NULL;
    }

    (*node)->number_links--;
}
