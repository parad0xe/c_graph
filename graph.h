
#include "./types.h"

#ifndef DATA_STRUCT_GRAPH_H
#define DATA_STRUCT_GRAPH_H

struct _graph_link;
struct _graph_node;

typedef struct _graph_link {
    int link_id;
    int value;
    struct _graph_node *next_node;
} GraphLinkElement, *GraphLink;

typedef struct _graph_node {
    char *name;
    int number_links;
    struct _graph_link **links;

} GraphNodeElement, *GraphNode;

typedef struct _graph {
    int number_nodes;
    int number_odd_nodes;
    struct _graph_node **nodes;
    Bool oriented;
    Bool related;
    Bool eulerian_string;
    Bool eulerian_cycle;
} GraphElement, *Graph;

// ---------------------------------------------

Graph create_graph(Bool oriented);
Bool is_empty_graph(Graph graph);
void print_graph(Graph graph);
void clear_graph(Graph *graph);
void destroy_graph(Graph *graph);

// ---------------------------------------------

GraphNode add_graph_node(Graph *graph, const char* name);
Bool is_empty_graph_node(GraphNode node);
void remove_graph_node(Graph *graph, GraphNode *node);

// ---------------------------------------------

GraphLink add_graph_link(Graph *graph, GraphNode *node, GraphNode *next_node, const int value);
void remove_graph_link(Graph *graph, GraphNode *node, GraphLink *link);

#endif //DATA_STRUCT_GRAPH_H
