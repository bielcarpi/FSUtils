#include "tree.h"

struct TreeNode * TREE_addChild(struct TreeNode *parent, char *name){
    // Add a child to the parent node
    struct TreeNode * node = (struct TreeNode *) malloc(sizeof(struct TreeNode));
    // Copy the name of the node
    node->name = (char *) malloc(strlen(name) * sizeof(char));
    strcpy(node->name, name);
    node->numChilds = 0;
    parent->numChilds++;

    //Reallocate memory for the array of child nodes in the parent
    if(parent->numChilds == 1)
        parent->child = (struct TreeNode **) malloc(parent->numChilds * sizeof(struct TreeNode *));
    else
        parent->child = (struct TreeNode **) realloc(parent->child, parent->numChilds * sizeof(struct TreeNode *));

    parent->child[parent->numChilds - 1] = node;
    return node;
}

void printNode(struct TreeNode * node, int level) {
    for (int i = 0; i < level - 1; i++)
        printf("│   ");  // Print vertical bars with indentation

    if (level > 0) printf("├── ");  // Print horizontal bar for non-root nodes
    //else if (last) printf("└── ");  // Print horizontal bar for the last child node

    if(node->name != NULL) printf("%s\n", node->name);  // Print the name of the node

    // Recursively print the child nodes
    for (int i = 0; i < node->numChilds; i++)
        printNode(node->child[i], level + 1);
}

void TREE_print(struct TreeNode *root) {
    if (root == NULL) {
        printf("Tree is empty.\n\n");
        return;
    }

    printf("\n");
    printNode(root, 0);  // Start printing from the root node at level 0
    printf("\n\n");
}

void TREE_free(struct TreeNode * root) {
    if (root == NULL) return;

    // Recursively free the child nodes
    for (int i = 0; i < root->numChilds; i++){
        TREE_free(root->child[i]);

        //Free the node itself (this is done here because the recursive call above can't free the node itself)
        free(root->child[i]);
    }

    // Free the name of the node
    if(root->name != NULL) free(root->name);

    // Free the array of child nodes
    free(root->child);
}