#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include "packing.h"

int max(int x, int y) {
	if(x > y)
		return x;
	else
		return y;
}

void get_dims(TreeNode* node, int level) {
	if(!node)
		return;
	if(level == 2)
		return;
	if(!level)
		level--;
	get_dims(node -> left, level + 1);
	get_dims(node -> right, level + 1);
	if(isalpha((node -> label)[0])) {
		if((node -> label)[0] == 'V') {
			node -> width = node -> left -> width + node -> right -> width;
			node -> height = max(node -> left -> height, node -> right -> height);
		}
		else {
			node -> width = max(node -> left -> width, node -> right -> width);
			node -> height = node -> left -> height + node -> right -> height;
		}
	}
}

TreeNode* read_lines(FILE* fptr) {
	int num_lines = 0;
	for(char ch = getc(fptr); ch != EOF; ch = getc(fptr)) {
		if(ch == '\n') {
			num_lines++;
		}
	}
	fseek(fptr, 0, SEEK_SET);
	TreeNode** stack = malloc(sizeof(TreeNode*) * num_lines);
	int stack_ctr = 0;
	for(char ch = getc(fptr); ch != EOF; ch = getc(fptr)) {
		TreeNode* node = malloc(sizeof(*node));
		char* lab = malloc(sizeof(char) * 11); // label string
		if(isdigit(ch)) {
			int digit_ctr = 0;
			while(ch != '(') {
				lab[digit_ctr] = ch;
				ch = getc(fptr);
				digit_ctr++;
			}
			lab[digit_ctr] = '\0';
			node -> label = lab;
		}
		else {
			lab[0] = ch;
			lab[1] = '\0';
			ch = getc(fptr);
			node -> label = lab;
		}
		if(ch != '\n') {
			ch = getc(fptr);
			int wid = 0;
			while(ch != ',') {
				int dig = ch - '0';
				wid = dig + 10 * wid;
				ch = getc(fptr);
			}
			node -> width = wid;
			ch = getc(fptr);
			int hgt = 0;
			while(ch != ')') {
				int dig = ch - '0';
				hgt = dig + 10 * hgt;
				ch = getc(fptr);
			}
			node -> height = hgt;
			ch = getc(fptr);
		}
		node -> x = 0;
		node -> y = 0;
		node -> visited = 0;
		// if leaf
		if(isdigit((node -> label)[0])) {
			stack[stack_ctr] = node;
			stack_ctr++;
			node -> right = NULL;
			node -> left = NULL;
		} // if not leaf
		else {
			stack_ctr--;
			node -> right = stack[stack_ctr];
			stack_ctr--;
			node -> left = stack[stack_ctr];
			stack[stack_ctr] = node;
			stack_ctr++;
			if((node -> label)[0] == 'V') {
				node -> width = node -> left -> width + node -> right -> width;
				node -> height = max(node -> left -> height, node -> right -> height);
			}
			else {
				node -> width = max(node -> left -> width, node -> right -> width);
				node -> height = node -> left -> height + node -> right -> height;
			}
		}
	}
	TreeNode* head = *stack;
	free(stack);
	return head;
}

void preorder_write(TreeNode* node, FILE* fptr2) {
	while(node) {
		if(isalpha((node -> label)[0])) {
			fprintf(fptr2, "%s\n", node -> label);
		}
		else {
			fprintf(fptr2, "%s(%d,%d)\n", node -> label, node -> width, node -> height);
		}
		preorder_write(node -> left, fptr2);
		node = node -> right;
	}
}

TreeNode* reroot(TreeNode* root, int dir, int dir2, TreeNode* ogtree, int* final_dir, int* final_dir2) {
	if(dir && !dir2) {
		if(!(root -> right -> left) || (root == ogtree))
			return root;
		TreeNode* oghead = root;
		TreeNode* moved_stree = root -> right -> right;
		TreeNode* newhead = root -> right;
		root = newhead;
		root -> right = oghead;
		root -> right -> right = moved_stree;
		*final_dir = dir;
		*final_dir2 = dir2;
	} else if(!dir && dir2) {
		if(!(root -> left -> right) || (root == ogtree))
			return root;
		TreeNode* oghead = root;
		TreeNode* moved_stree = root -> left -> left;
		TreeNode* newhead = root -> left;
		root = newhead;
		root -> left = oghead;
		root -> left -> left = moved_stree;
		*final_dir = dir;
		*final_dir2 = dir2;
	} else if(dir && dir2) {
		if(!(root -> right -> right) || (root == ogtree))
			return root;
		TreeNode* oghead = root;
		TreeNode* moved_stree = root -> right -> left;
		TreeNode* newhead = root -> right;
		root = newhead;
		root -> left = oghead;
		root -> left -> right = moved_stree;
		*final_dir = dir;
		*final_dir2 = dir2;
	} else {
		if(!(root -> left -> left) || (root == ogtree))
			return root;
		TreeNode* oghead = root;
		TreeNode* moved_stree = root -> left -> right;
		TreeNode* newhead = root -> left;
		root = newhead;
		root -> right = oghead;
		root -> right -> left = moved_stree;
		*final_dir = dir;
		*final_dir2 = dir2;
	}
	return root;
}

TreeNode* zigzag(TreeNode* root, int dir, int dir2, TreeNode* ogtree, int* final_dir, int* final_dir2) {
	if(dir && !dir2) {
		if(!(root -> right -> left) || (root == ogtree)) {
			*final_dir = 0;
			*final_dir2 = 1;
			return root;
		}
		root = reroot(root, 1, 0, ogtree, final_dir, final_dir2);
	} else {
		if(!(root -> left -> right) || (root == ogtree)) {
			*final_dir = 1;
			*final_dir2 = 0;
			return root;
		}
		root = reroot(root, 0, 1, ogtree, final_dir, final_dir2);
	}
	root = zigzag(root, !dir, !dir2, ogtree, final_dir, final_dir2);
	return root;
}

TreeNode* get_all_packing(TreeNode* root, int dir, int dir2, TreeNode* ogtree, int* final_dir, int* final_dir2, FILE* fptr4) {
	ogtree = root;
	//printf("Here: %s\n", root -> label);
	if(dir && !dir2) {
		if(!(root -> right -> left) || (root -> right -> left -> visited)) {
			return root;
		}
		root = reroot(root, 1, 0, NULL, final_dir, final_dir2);
		get_dims(root, 0);
		root -> left -> visited = 1;
		fprintf(fptr4, "%s(%d,%d)\n", root -> left -> label, root -> width, root -> height);
		//printf("Node visited: %s(%d,%d)\n", root -> left -> label, root -> width, root -> height);
		if(!(root -> left -> right)) {
			root = reroot(root, dir, dir2, NULL, final_dir, final_dir2);
			return root;
		}
	} else if(!dir && dir2) { 
		if(!(root -> left -> right) || (root -> left -> right -> visited)) {
			return root;
		}
		root = reroot(root, 0, 1, NULL, final_dir, final_dir2);
		get_dims(root, 0);
		root -> right -> visited = 1;
		fprintf(fptr4, "%s(%d,%d)\n", root -> right -> label, root -> width, root -> height);
		//printf("Node visited: %s(%d,%d)\n", root -> right -> label, root -> width, root -> height);
		if(!(root -> right -> left)) {
			root = reroot(root, dir, dir2, NULL, final_dir, final_dir2);
			return root;
		} else {
			root = get_all_packing(root, 1, 0, ogtree, final_dir, final_dir2, fptr4);
			root = get_all_packing(root, 1, 1, ogtree, final_dir, final_dir2, fptr4);
			root = reroot(root, dir, dir2, NULL, final_dir, final_dir2);
			return root;
		}
	} else if(dir && dir2) {
		if(!(root -> right -> right) || (root -> right -> right -> visited)) {
			return root;
		}
		root = reroot(root, 1, 1, NULL, final_dir, final_dir2);
		get_dims(root, 0);
		root -> right -> visited = 1;
		fprintf(fptr4, "%s(%d,%d)\n", root -> right -> label, root -> width, root -> height);
		//printf("Node visited: %s(%d,%d)\n", root -> right -> label, root -> width, root -> height);
		if(!(root -> right -> right)) {
			root = reroot(root, 0, 0, NULL, final_dir, final_dir2);
			return root;
		} else {
			root = get_all_packing(root, 1, 0, ogtree, final_dir, final_dir2, fptr4);
			root = get_all_packing(root, 1, 1, ogtree, final_dir, final_dir2, fptr4);
			root = reroot(root, 0, 0, NULL, final_dir, final_dir2);
			return root;
		}
	} else {
		if(!(root -> left -> left) || (root -> left -> left -> visited)) {
			return root;
		}
		root = reroot(root, 0, 0, NULL, final_dir, final_dir2);
		get_dims(root, 0);
		root -> left -> visited = 1;
		fprintf(fptr4, "%s(%d,%d)\n", root -> left -> label, root -> width, root -> height);
		//printf("Node visited: %s(%d,%d)\n", root -> left -> label, root -> width, root -> height);
		if(!(root -> left -> left)) {
			root = reroot(root, 1, 1, NULL, final_dir, final_dir2);
			return root;
		}
	}	
	root = get_all_packing(root, 0, 0, ogtree, final_dir, final_dir2, fptr4);	
	root = get_all_packing(root, 0, 1, ogtree, final_dir, final_dir2, fptr4);
	
	if(root -> left -> visited) {
		if(!dir)
			root = reroot(root, 1, 1, NULL, final_dir, final_dir2);
		else
			root = reroot(root, 1, 0, NULL, final_dir, final_dir2);
		return root;
	}
	root = get_all_packing(root, 1, 0, ogtree, final_dir, final_dir2, fptr4);
	root = get_all_packing(root, 1, 1, ogtree, final_dir, final_dir2, fptr4);
	return root;
}