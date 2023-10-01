#ifndef TREE_H
#define TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct TreeNode {
    char *name;
    struct TreeNode ** child;
    int numChilds;
};

//Adds a child to the tree, given the root node and the name of the child
struct TreeNode * TREE_addChild(struct TreeNode *parent, char *name);

//Prints the tree, given the root node
void TREE_print(struct TreeNode *root);

//Frees the tree, given the root node
void TREE_free(struct TreeNode *root);

#endif
