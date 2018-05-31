#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Dijkstra.h"
#include "PQ.h"
#include <limits.h>

//the following function is for creating new PQ nodes, since ItemPQ is a node, not a pointer (which complicates adding to and dequeueing from PQ)
ItemPQ *newPQnode(int key, int value) {
    ItemPQ *new = malloc(sizeof(ItemPQ));
    if (new == NULL) {
        fprintf(stderr, "Error!\n");
        return NULL;
    }
    new->key = key;
    new->value = value;
    return new;
}

PredNode *newpredecessor(int v) {
    PredNode *new = malloc(sizeof(PredNode));
    if (new == NULL) {
        fprintf(stderr, "Error!\n");
        return NULL;
    }
    new->v = v;
    new->next = NULL;
    return new;
}

void freepredecessor(PredNode *delete) {
    free(delete);
}

ShortestPaths dijkstra(Graph g, Vertex v) {
    ShortestPaths *paths; //the struct holding the shortest paths info
    paths = malloc (sizeof (struct ShortestPaths));
    if (paths == NULL) {
        fprintf(stderr, "Error!\n");
		exit(EXIT_FAILURE);
    }
    paths->noNodes = numVerticies(g);
    paths->src = v;
    paths->dist = malloc(paths->noNodes*sizeof(int));
    if (paths->dist == NULL) {
        fprintf(stderr, "Error!\n");
        exit(EXIT_FAILURE);
    }
    paths->pred = malloc(paths->noNodes*sizeof(PredNode *));
    if (paths->pred == NULL) {
        fprintf(stderr, "Error!\n");
        exit(EXIT_FAILURE);
    }
    
    PQ PathPQ = newPQ(); //the priority queue for inspecting nodes
    AdjList current; //our pointer for scanning through the graph
    int i = 0; //an iterator for looking at vertices
    ItemPQ tmp; //used to ger dijkstra result then & operator is applied
    ItemPQ *temp; //queue objects for interacting with our priority queue
    ItemPQ *save;
    int new_dist; //an integer to hold distances for comparison
    PredNode *predecessor; //a pointer to a predecessor that will be used to insert into the pred array
    int current_vertex = 0;
    int neighbour = 0;
    PredNode *scan; //for scanning through the predecessor list of a vertex
    PredNode *delete; //used for clearing nodes from predecessor lists
    int append = 0;

    while (i < numVerticies(g)) {
        if (i == v) { //this is the src node
            temp = newPQnode(i, 0);
            paths->dist[i] = 0;
        }
        else {
            temp = newPQnode(i, INT_MAX); //to simulate infinity
            paths->dist[i] = INT_MAX;
        }
        addPQ(PathPQ, *temp);
        i++;
    }

    //at this point, our priority queue has all vertices in the graph g, and will grab vertex v first

    while (!PQEmpty(PathPQ)) {
        tmp = dequeuePQ(PathPQ); //get the vertex with the smallest distance
        temp = &tmp;
        current_vertex = temp->key;
        current = outIncident(g, current_vertex); //the list of all vertices pointed to by our current node (outgoing edges)
        while (current != NULL) {
            neighbour = current->w;
            new_dist = paths->dist[current_vertex] + current->weight;
            if (new_dist < paths->dist[neighbour]) {
                //we have found a shorter path, so clear the whole pred list for neighbour and insert the new predecessor current_vertex
                save = newPQnode(neighbour, new_dist); //save is a queue node for neighbour vertex w, with new shortest distance alt
                updatePQ(PathPQ, *save); //update w's distance in our priority queue to ensure acccurate inspection later

                paths->dist[neighbour] = new_dist; //update our distance array

                if (paths->pred[neighbour] == NULL) {
                    //we have not yet found a shorter path than infinity for this node, so insert a new predecessor
                    predecessor = newpredecessor(current_vertex);
                    paths->pred[neighbour] = predecessor;
                } else {
                    //clear the entire pred list, then add the new predecessor
                    scan = paths->pred[neighbour];
                    while (scan != NULL) {
                        delete = scan;
                        scan = scan->next;
                        freepredecessor(delete);
                    }
                    predecessor = newpredecessor(current_vertex);
                    paths->pred[neighbour] = predecessor; //the list contains one node, the new, shortest predecessor
                }
            } else if (new_dist == paths->dist[neighbour]) {
                //we have found a path of equal length, so append it to the pred list for current_vertex
                predecessor = newpredecessor(current_vertex);
                append = 1;
                for (scan = paths->pred[neighbour]; scan->next != NULL; scan = scan->next) { //reach the last node of the pred list for neighbour
                    if (scan->v == current_vertex) {
                        append = 0;
                        break;
                    }
                }
                if (append == 1) {
                    scan->next = predecessor;
                }
            }
            current = current->next;
        }
    }
    return *paths;

}

void  showShortestPaths(ShortestPaths p) {
    //TODO
}

void  freeShortestPaths(ShortestPaths p) {
    ShortestPaths *delete = &p;
    free(delete->dist); //free the distance array

    int i = 0;
    PredNode *scan;
    PredNode *scan2;
    while (i < delete->noNodes) { //free the list for each vertex in the pred array
        scan = delete->pred[i];
        while (scan != NULL) {
            scan2 = scan;
            scan = scan->next;
            free(scan2);
        }
        i++;
    }
    free(delete->pred);
    free(delete); //free the shortestpaths struct itself
}