#include <stdio.h>
#include <stdlib.h>

typedef struct _TreeNode {
	char* label;
	int width;
	int height;
	int x;
	int y;
	int visited;
	struct _TreeNode* left;
	struct _TreeNode* right;
} TreeNode;

TreeNode* read_lines(FILE* fptr);
void preorder_write(TreeNode* node, FILE* fptr2);
TreeNode* reroot(TreeNode* root, int dir, int dir2, TreeNode* ogtree, int* final_dir, int* final_dir2);
TreeNode* zigzag(TreeNode* root, int dir, int dir2, TreeNode* ogtree, int* final_dir, int* final_dir2);
TreeNode* get_all_packing(TreeNode* root, int dir, int dir2, TreeNode* ogtree, int* final_dir, int* final_dir2, FILE* fptr4);
void get_dims(TreeNode* node, int level);