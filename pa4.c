#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <limits.h>
#include "packing.h"

void destroy_tree(TreeNode** a_root) {
	if(*a_root != NULL) {
		destroy_tree(&((*a_root) -> left));
		destroy_tree(&((*a_root) -> right));
		free((*a_root) -> label);
		free(*a_root);
		*a_root = NULL;
	}
}

int main(int argc, char** argv) {
	if (argc != 5)
		return EXIT_FAILURE;
	FILE * fptr = fopen(argv[1], "r");
	if(!fptr){
		return EXIT_FAILURE;
	}
	FILE * fptr2 = fopen(argv[2], "w");
	if(!fptr2){
		fclose(fptr);
		return EXIT_FAILURE;
	}
	FILE * fptr3 = fopen(argv[3], "w");
	if(!fptr3){
		fclose(fptr);
		fclose(fptr2);
		return EXIT_FAILURE;
	}
 	FILE * fptr4 = fopen(argv[4], "w");
	if(!fptr4){
		fclose(fptr);
		fclose(fptr2);
		fclose(fptr3);
		return EXIT_FAILURE;
	}
	
	TreeNode* tree = read_lines(fptr); // make tree from postorder and determine dimensions
	
	int final_dir, final_dir2;
	TreeNode* left_reroot = zigzag(tree, 0, 1, NULL, &final_dir, &final_dir2);
	preorder_write(left_reroot, fptr2);
	TreeNode* ogtree = zigzag(left_reroot, final_dir, final_dir2, tree, &final_dir, &final_dir2);
	
	TreeNode* right_reroot = zigzag(ogtree, 1, 0, NULL, &final_dir, &final_dir2);
	preorder_write(right_reroot, fptr3);
	ogtree = zigzag(right_reroot, final_dir, final_dir2, tree, &final_dir, &final_dir2);
	
	fprintf(fptr4, "%s\n", ogtree -> label);
	fprintf(fptr4, "%s\n", ogtree -> left -> label);
	get_dims(ogtree, 0);
	//printf("Node visited: %s\n", ogtree -> label);
	//printf("Node visited: %s\n", ogtree -> left -> label);
	get_all_packing(ogtree, 0, 0, ogtree, &final_dir, &final_dir2, fptr4);
	get_all_packing(ogtree, 0, 1, ogtree, &final_dir, &final_dir2, fptr4);
	fprintf(fptr4, "%s\n", ogtree -> right -> label);
	//printf("Node visited: %s\n", ogtree -> right -> label);
	get_all_packing(ogtree, 1, 0, ogtree, &final_dir, &final_dir2, fptr4);
	get_all_packing(ogtree, 1, 1, ogtree, &final_dir, &final_dir2, fptr4);
	
	fclose(fptr);
	fclose(fptr2);
	fclose(fptr3);
	fclose(fptr4);
	destroy_tree(&ogtree);
	return EXIT_SUCCESS;
}