#include "imgopt/util.h"
#include "imgopt/signal_network.h"
#include "imgopt/thpool.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>

static struct Edge* addEdge(struct Node *from, struct Node *to)
{
    struct Edge *edge = xmalloc(sizeof *edge);
    edge->inNode = from;
    edge->outNode = to;
    if (from != NULL)
        array_append(from->outEdges, edge);
    if (to != NULL)
        array_append(to->inEdges, edge);
    return edge;
}

static struct Node* createNode(NodeFunction step_func, void *user_data)
{
    struct Node *node = xmalloc(sizeof *node);
    *node = (struct Node){
            .inEdges = array_new(),
            .outEdges = array_new(),
            .step_func = step_func,
            .user_data = user_data
    };
    return node;
}

static enum Error dftFunction(struct Node *node, int8_t offset)
{
    double complex result = 0;
    const int k = *((int*)node->user_data);
    const int n = node->inEdges->count;
    for (int index = 0; index < n; ++index)
    {
        struct Edge *edge = node->inEdges->values[index];
        const complex double value = edge->values[1-offset];
//        printf("Input edge %d has value (%f%+fi)\n", index, creal(value), cimag(value));
        result += value * cexp(-I * 2 * M_PI * index / k);
    }
    for (int out_index = 0; out_index < node->outEdges->count; ++out_index)
    {
        struct Edge *outEdge = node->outEdges->values[out_index];
        outEdge->values[offset] = result;
    }
    return OK;
}


static enum Error pushFunction(struct Node *node, int8_t offset)
{
    if (node->inEdges->count != 1)
        return INVALID_ARG;
    struct Edge *inEdge = node->inEdges->values[0];
    const complex double value = inEdge->values[1-offset];
//    printf("Pushing value (%f%+fi) to %d edges\n", creal(value), cimag(value), node->outEdges->count);
    for (int out_index = 0; out_index < node->outEdges->count; ++out_index)
    {
        struct Edge *out = node->outEdges->values[out_index];
        out->values[offset] = value;
    }
    return OK;
}


//static struct Array* establishNodeOrder(struct Network *network)
//{
//    struct Array *result = array_new();
//    struct Array *all_edges = array_new();
//    struct LinkedList *nodesToCheck = xmalloc(sizeof *nodesToCheck);
//    const double dummy = -1;
//    network->inputEdge->value = dummy;
//    linkedlist_appendRight(nodesToCheck, network->inputEdge->outNode);
//    while (nodesToCheck->head != NULL)
//    {
//        struct Node *node = linkedlist_popLeft(nodesToCheck);
//        bool runnable = true;
//        for (int inEdge_ind = 0; inEdge_ind < node->inEdges->count; ++inEdge_ind)
//        {
//            struct Edge *inEdge = node->inEdges->values[inEdge_ind];
//            if (inEdge->value != dummy)
//            {
//                runnable = false;
//                break;
//            }
//        }
//        if (runnable)
//        {
//            array_append(result, node);
//            for (int outEdge_ind = 0; outEdge_ind < node->outEdges->count; ++outEdge_ind)
//            {
//                struct Edge *outEdge = node->outEdges->values[outEdge_ind];
//                outEdge->value = dummy;
//                array_append(all_edges, outEdge);
//                if (outEdge->outNode != NULL)
//                    linkedlist_appendRight(nodesToCheck, outEdge->outNode);
//            }
//        }
//    }
//    for (int edge_ind = 0; edge_ind < all_edges->count; ++edge_ind)
//    {
//        struct Edge *edge = all_edges->values[edge_ind];
//        edge->value = 0;
//    }
//    array_reverse(result);
//    return result;
//}

struct WrapperArgs
{
    struct Node *node;
    int8_t offset;
};

static void runNodeWrapper(void *arg)
{
    assert(arg != NULL);
    struct WrapperArgs *args = arg;
    args->node->step_func(args->node, args->offset);
}

static enum Error runNetwork(
        struct Network *network,
        SignalGenerator generator,
        void *generator_args,
        complex double *outputs,
        int iteration_count)
{
    threadpool pool = thpool_init(32);
    int8_t offset = 1;
    int num_nodes = network->allNodes->count;
    struct WrapperArgs *allArgs = xmalloc(num_nodes * sizeof *allArgs);
    for (int iter = 0; iter < iteration_count; ++iter)
    {
        const complex double input = generator(iter, generator_args);
//        printf("Passing in input value (%f%+fi) at iteration %d\n", creal(input), cimag(input), iter);
        network->inputEdge->values[1-offset] = input;
        for (int node_ind = 0; node_ind < network->allNodes->count; ++node_ind)
        {
            allArgs[node_ind].node = network->allNodes->values[node_ind];
            allArgs[node_ind].offset = offset;
            thpool_add_work(pool, runNodeWrapper, &allArgs[node_ind]);
//            runNodeWrapper(&allArgs[node_ind]);
        }
        thpool_wait(pool);
        const complex double result = network->outputEdge->values[offset];
        outputs[iter] = result;
        if (iter % 100 == 0)
            printf("Finished iter %d with result %f\n", iter, cabs(result));
        offset = (int8_t)1 - offset;
    }
    return OK;
}


extern struct Network* createTestNetwork()
{
    int *k = xmalloc(sizeof *k);
    *k = 5;
    struct Node *rootNode = createNode(pushFunction, NULL);
    struct Node *dftNode = createNode(dftFunction, k);
    struct Node *lastPushNode = rootNode;
    struct Array *allNodes = array_new();
    array_append(allNodes, rootNode);
    array_append(allNodes, dftNode);
    for (int ind = 0; ind < 1000; ++ind)
    {
        struct Node *pushNode = createNode(pushFunction, NULL);
        addEdge(lastPushNode, pushNode);
        addEdge(pushNode, dftNode);
        lastPushNode = pushNode;
        array_append(allNodes, pushNode);
    }
    struct Network *network = xmalloc(sizeof *network);
    network->inputEdge = addEdge(NULL, rootNode);
    network->outputEdge = addEdge(dftNode, NULL);
    network->allNodes = allNodes;
    return network;
}

struct SineWaveData
{
    double omega;
    double sample_rate;
};

static complex double sine_generator(int iter, void *user_data)
{
    struct SineWaveData *data = user_data;
    return sin(2*M_PI*data->omega*iter/data->sample_rate);
}

extern int main()
{
    printf("Test.\n");
    struct Network *network = createTestNetwork();
    int iteration_count = 120000;
    double *inputs = xmalloc_atomic(iteration_count * sizeof *inputs);
    for (int input_ind = 0; input_ind < iteration_count; ++input_ind)
    {
        inputs[input_ind] = sin(2*M_PI*input_ind*5/iteration_count);
    }
    complex double *outputs = xmalloc_atomic(iteration_count * sizeof *outputs);
    struct SineWaveData generator_args = {.omega = 1.0/5, .sample_rate=1};
    runNetwork(network, sine_generator, &generator_args, outputs, iteration_count);
    return 0;
}

