import numpy as np


class Main:
    def __init__(self):
        pass

    def main(self):
        # import ipdb; ipdb.set_trace()
        signal = np.ones(10)
        node0 = Node()
        node1 = Node()
        graph = Graph({0: node0, 1: node1})
        for element in signal:
            if element == 0:
                node0.setnext()
                node1.unsetnext()
            else:
                node0.unsetnext()
                node1.setnext()
            graph.propagate((node0, node1))
            graph.flip()
            print(f"Number of nodes: {len(graph._nodes)}")


class Graph:
    def __init__(self, rootnodes):
        self._nodes = rootnodes

    def propagate(self, nodes, others=None):
        nextnodes = []
        if others is None:
            others = nodes
        print(f"Propagating {len(nodes)} nodes ({len(others)} others)")
        for ind, node in enumerate(nodes):
            identitylink = self._nodes.get(node, None)
            if identitylink is not None:
                nextnodes.append(identitylink)
            if node.isset():
                if identitylink is None:
                    identitylink = Node()
                    self._nodes[node] = identitylink
                identitylink.setnext()
            elif identitylink is not None:
                identitylink.unsetnext()
                nextnodes.append(identitylink)
            for node2 in others:
                if node == node2:
                    continue
                pair = tuple(sorted((node, node2)))
                pairlink = self._nodes.get(pair, None)
                if pairlink is not None:
                    nextnodes.append(pairlink)
                if node.isset() and node2.isset():
                    if pairlink is None:
                        pairlink = Node()
                        self._nodes[pair] = pairlink
                    pairlink.setnext()
                elif pairlink is not None:
                        pairlink.unsetnext()
                        nextnodes.append(pairlink)
        if len(nextnodes) > 0:
            nextnodes = tuple(nextnodes)
            self.propagate(nextnodes, nextnodes + others)

    def flip(self):
        for node in self._nodes.values():
            node.flip()


class Node:
    def __init__(self):
        # self._nodes = nodes
        # self._depth = depth
        self._counter = 0
        self._value = 0
        self._nextval = 0

    def setnext(self):
        self._counter += 1
        self._nextval = 1

    def unsetnext(self):
        self._nextval = 0

    def isset(self):
        return self._value

    def flip(self):
        self._value = self._nextval
        self._nextval = 0

    def __lt__(self, other):
        return id(self) < id(other)

    def __gt__(self, other):
        return id(self) > id(other)

    def __eq__(self, other):
        return id(self) == id(other)

    def __hash__(self):
        return id(self)

    def __str__(self):
        return f"{id(self)} ({self._counter})"

    def __repr__(self):
        return f"Node({id(self)}, {self._counter})"


if __name__ == '__main__':
    Main().main()
