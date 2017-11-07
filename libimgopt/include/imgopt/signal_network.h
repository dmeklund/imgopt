#ifndef _IMGOPT_SIGNAL_NETWORK_H
#define _IMGOPT_SIGNAL_NETWORK_H

#include "imgopt/util.h"

#include <complex.h>

struct Node;
typedef enum Error (*NodeFunction)(struct Node *node, int8_t offset);
typedef complex double (*SignalGenerator)(int iter, void *user_data);

struct Node
{
    NodeFunction step_func;
    struct Array *inEdges; // struct Edge *
    struct Array *outEdges; // struct Edge *
    void *user_data;
};

struct Edge
{
    double complex values[2];
    struct Node *inNode;
    struct Node *outNode;
};

struct Network
{
    struct Edge *inputEdge;
    struct Edge *outputEdge;
    struct Array *allNodes;
};

#endif /* _IMGOPT_SIGNAL_NETWORK_H */
