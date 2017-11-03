#include <gc.h>
#include <complex.h>
#include <math.h>

#define xmalloc GC_MALLOC

enum Error
{
    OK,
    INVALID_ARG
};

typedef enum Error (*NodeFunction)(struct Node *node);

struct Node
{
    NodeFunction step_func;
    struct Edge *inEdges;
    struct Edge *outEdges;
    int numInEdges;
    int numOutEdges;
    void *user_data;
};

struct Edge
{
    double complex value;
    struct Node *inNode;
    struct Node *outNode;
};


static enum Error dftFunction(struct Node *node)
{
    double complex result = 0;
    const int k = *((int*)node->user_data);
    const int n = node->numInEdges;
    for (int index = 0; index < n; ++index)
    {
        result += node->inEdges[index].value * cexp(-I * 2 * M_PI * n * index / node->numInEdges);
    }
    for (int out_index = 0; out_index < node->numOutEdges; ++out_index)
    {
        node->outEdges[out_index].value = result;
    }
    return OK;
}


static enum Error pushFunction(struct Node *node)
{
    if (node->numInEdges != 1)
        return INVALID_ARG;
    for (int out_index = 0; out_index < node->numOutEdges; ++out_index)
    {
        node->outEdges[out_index].value = node->inEdges[0].value;
    }
}


void createTestNetwork()
{
    struct Node *rootNode = xmalloc(sizeof *rootNode);
    struct Edge *rootEdge = xmalloc(sizeof *rootEdge);
    rootNode->numInEdges = 1;
    rootNode->inEdges = rootEdge;
    rootNode->outEdges = xmalloc(sizeof *rootNode->outEdges);
    rootNode->numOutEdges = 1;
    rootEdge->inNode = NULL;
    rootEdge->outNode = rootNode;
    struct Node *push1 = xmalloc(sizeof *push1);
    rootNode->outEdges[0].inNode = rootNode;
    rootNode->outEdges[0].outNode = push1;
    push1->numInEdges = 1;
    push1->inEdges = xmalloc(sizeof *push1->inEdges);
    push1->inEdges[0] = rootNode->outEdges[0];
    push1->numOutEdges = 2;
    push1->outEdges = xmalloc(2 * sizeof *push1->outEdges);
    push1->outEdges[0].
    struct Node *dftNode = xmalloc(sizeof *dftNode);
    dftNode->numInEdges = 10;
    dftNode->step_func = dftFunction;

}

