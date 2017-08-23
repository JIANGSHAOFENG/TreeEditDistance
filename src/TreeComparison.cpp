#include "TreeComparison.h"
#include "Tree.h"

#include <iostream>
#include <string.h>
#include <fstream>
#include <vector>
#include <stack>
#include <climits>

using namespace std;

TreeComparison::TreeComparison() {
}

TreeComparison::TreeComparison(Tree* A, Tree* B, SimiMatrix costModel) {
	A_ = A;
	B_ = B;
	costModel_ = costModel;

	treeSizeA = A_->getTreeSize();
	treeSizeB = B_->getTreeSize();

	int maxSize = treeSizeA < treeSizeB? treeSizeB + 1 : treeSizeA + 1;
	fn = new int[maxSize + 1];
	ft = new int[maxSize + 1];

	fn_ft_length = maxSize + 1;

	Free = new int*[treeSizeA];
	LeftA = new int*[treeSizeA];
	LeftB = new int*[treeSizeA];
	RightA = new int*[treeSizeA];
	RightB = new int*[treeSizeA];
	AllA = new int*[treeSizeA];
	AllB = new int*[treeSizeA];
	LeftAStrategies = new Strategy*[treeSizeA];
	LeftBStrategies = new Strategy*[treeSizeA];
	RightAStrategies = new Strategy*[treeSizeA];
	RightBStrategies = new Strategy*[treeSizeA];
	AllAStrategies = new Strategy*[treeSizeA];
	AllBStrategies = new Strategy*[treeSizeA];
	FreeStrategies = new Strategy*[treeSizeA];
	delta = new float*[treeSizeA];
	hasVisited = new bool*[treeSizeA];
	d = new float*[treeSizeA];

	for(int i = 0; i < treeSizeA; i++) {
		Free[i] = new int[treeSizeB];
		LeftA[i] = new int[treeSizeB];
		LeftB[i] = new int[treeSizeB];
		RightA[i] = new int[treeSizeB];
		RightB[i] = new int[treeSizeB];
		AllA[i] = new int[treeSizeB];
		AllB[i] = new int[treeSizeB];
		LeftAStrategies[i] = new Strategy[treeSizeB];
		LeftBStrategies[i] = new Strategy[treeSizeB];
		RightAStrategies[i] = new Strategy[treeSizeB];
		RightBStrategies[i] = new Strategy[treeSizeB];
		AllAStrategies[i] = new Strategy[treeSizeB];
		AllBStrategies[i] = new Strategy[treeSizeB];
		FreeStrategies[i] = new Strategy[treeSizeB];
		delta[i] = new float[treeSizeB];
		hasVisited[i] = new bool[treeSizeB];
		d[i] = new float[treeSizeB];
	}

	s = new float*[maxSize - 1];
	t = new float*[maxSize - 1];
	q = new float[maxSize - 1];
	for(int i = 0; i < maxSize - 1; i++) {
		s[i] = new float[maxSize - 1];
		t[i] = new float[maxSize - 1];
	}

	for(int i = 0; i < treeSizeA; i++) {
		for(int j = 0; j < treeSizeB; j++) {
			Free[i][j] = -1;
			LeftA[i][j] = -1;
			LeftB[i][j] = -1;
			RightA[i][j] = -1;
			RightB[i][j] = -1;
			AllA[i][j] = -1;
			AllB[i][j] = -1;
			delta[i][j] = 0.0f;
			hasVisited[i][j] = false;
		}
	}

	ou.open("out.txt");

};


void TreeComparison::deltaInit() {
	int treeSizeA = A_->getTreeSize();
	int treeSizeB = B_->getTreeSize();
	for(int i = 0; i < treeSizeA; i++) {
		Node* a = (*A_)[i];
		for(int j = 0; j < treeSizeB; j++) {
			Node* b = (*B_)[j];
			if(a->getSubTreeSize() == 1 && b->getSubTreeSize() == 1) {
				delta[i][j] = 0.0f;
			} else if(a->getSubTreeSize() == 1) {
				delta[i][j] = B_->preL_to_sumInsCost[j] - costModel_.ins(b->getLabel());
			} else if(b->getSubTreeSize() == 1) {
				delta[i][j] = A_->preL_to_sumDelCost[i] - costModel_.del(a->getLabel());
			}
		}
	}
};


void TreeComparison::computeSumInsAndDelCost(Tree* tree) {
	int treeSize = tree->getTreeSize();
	for(int i = 0; i < treeSize; i++) {
		int nodeForSum = treeSize - i - 1;// postOrder from bottom to up
		Node* node = (*tree)[nodeForSum];
		Node* parent = node->getParent();
	/*	cout << "Before" << endl;
		cout << "preL_to_sumInsCost[" << to_string(nodeForSum) << "] = " << to_string(tree->preL_to_sumInsCost[nodeForSum]) << endl;
		cout << "preL_to_sumDelCost[" << to_string(nodeForSum) << "] = " << to_string(tree->preL_to_sumDelCost[nodeForSum]) << endl;
		cout << "ins " << node->getLabel() << " " << costModel_.ins(node->getLabel()) << endl;
		cout << "del " << node->getLabel() << " " << costModel_.del(node->getLabel()) << endl;
		cout << "After" << endl;*/
		tree->preL_to_sumInsCost[nodeForSum] += costModel_.ins(node->getLabel());
		tree->preL_to_sumDelCost[nodeForSum] += costModel_.del(node->getLabel());
/*		cout << "preL_to_sumInsCost[" << to_string(nodeForSum) << "] = " << to_string(tree->preL_to_sumInsCost[nodeForSum]) << endl;
		cout << "preL_to_sumDelCost[" << to_string(nodeForSum) << "] = " << to_string(tree->preL_to_sumDelCost[nodeForSum]) << endl;*/
		if(parent != NULL) {
			/*cout << "Update Parent Before" << endl;
			cout << "preL_to_sumInsCost[" << to_string(parent->getID()) << "] = " << to_string(tree->preL_to_sumInsCost[parent->getID()]) << endl;
			cout << "preL_to_sumDelCost[" << to_string(parent->getID()) << "] = " << to_string(tree->preL_to_sumDelCost[parent->getID()]) << endl;*/
			tree->preL_to_sumInsCost[parent->getID()] += tree->preL_to_sumInsCost[node->getID()];
			tree->preL_to_sumDelCost[parent->getID()] += tree->preL_to_sumDelCost[node->getID()];
/*			cout << "Update Parent After" << endl;
			cout << "preL_to_sumInsCost[" << to_string(parent->getID()) << "] = " << to_string(tree->preL_to_sumInsCost[parent->getID()]) << endl;
			cout << "preL_to_sumDelCost[" << to_string(parent->getID()) << "] = " << to_string(tree->preL_to_sumDelCost[parent->getID()]) << endl;*/
		}
	}

};


Strategy** TreeComparison::APTED_ComputeOptStrategy_postL() {
	Strategy** S = new Strategy*[treeSizeA];
	float** cost1_L = new float*[treeSizeA];//in postL order
    float** cost1_R = new float*[treeSizeA];//in postL order
    float** cost1_I = new float*[treeSizeA];//in postL order
    int pathIDOffset = treeSizeA;

	for(int i = 0; i < treeSizeA; i++) {
		S[i] = new Strategy[treeSizeB];
		cost1_L[i] = new float[treeSizeB];
		cost1_R[i] = new float[treeSizeB];
		cost1_I[i] = new float[treeSizeB];
	}

	for(int i = 0; i < treeSizeA; i++) {
		for(int j = 0; j < treeSizeB; j++) {
			Strategy s;
			s.setLeaf(-1);
			s.setTreeToDecompose(0);
			S[i][j] = s;
		}
	}

	float* cost2_L = new float[treeSizeB];//in postL order
    float* cost2_R = new float[treeSizeB];//in postL order
    float* cost2_I = new float[treeSizeB];//in postL order
    int* cost2_path = new int[treeSizeB];//in postL order
    float* leafRow = new float[treeSizeB];

	float minCost = 0x7fffffffffffffffL;
	int strategyPath = -1;

	int size_i, size_j;

	stack<float*> rowsToReuse_L;
    stack<float*> rowsToReuse_R;
    stack<float*> rowsToReuse_I;

	for(int i = 0 ; i < treeSizeA; i++) {


		int i_in_preL = (A_)->postL_to_preL[i];
		size_i = (*A_)[i_in_preL]->getSubTreeSize();

		bool is_i_leaf = size_i == 1? true : false;
		Node* parent_i = (*A_)[i_in_preL]->getParent();
		int parent_i_preL;
		int parent_i_postL;

		int strategyLeftIndex = i_in_preL;
		int strategyRightIndex;
		int strategy_parent_i_LeftIndex, strategy_parent_i_RightIndex;

		int cost_L_LeftIndex, cost_L_RightIndex;
		int cost_R_LeftIndex, cost_R_RightIndex;
		int cost_I_LeftIndex, cost_I_RightIndex;

		int cost_L_parent_i_LeftIndex, cost_L_parent_i_RightIndex;
		int cost_R_parent_i_LeftIndex, cost_R_parent_i_RightIndex;
		int cost_I_parent_i_LeftIndex, cost_I_parent_i_RightIndex;

		if(parent_i != NULL) {
			parent_i_preL = parent_i->getID();
			parent_i_postL = (A_)->preL_to_postL[parent_i_preL];
		}


        int leftPath_i = (A_)->preL_to_lid[i_in_preL];
        int rightPath_i = (A_)->preL_to_rid[i_in_preL];
      	int i_leftmost_forest = (*A_)[i_in_preL]->getLeftmostForestNum();
      	int i_rightmost_forest = (*A_)[i_in_preL]->getRightmostForestNum();
      	int i_special_forest = (*A_)[i_in_preL]->getSpecialForestNum();

      	if(is_i_leaf) {
      		if(DEBUG) {
      			ou << to_string(i_in_preL) << " is a leaf" << endl;
      		}
        	cost1_L[i] = leafRow;
        	cost1_R[i] = leafRow;
        	cost1_I[i] = leafRow;
        	for(int j = 0; j < treeSizeB; j++) {
        		strategyRightIndex = (B_)->postL_to_preL[j];
        		if(DEBUG) {
        			ou << "set S[" << to_string(strategyLeftIndex) << ", " << to_string(strategyRightIndex) << "] = " << to_string(i_in_preL) << endl;
        		}
          		S[strategyLeftIndex][strategyRightIndex].setLeaf(i_in_preL);//decomposit the left tree
          		S[strategyLeftIndex][strategyRightIndex].setTreeToDecompose(0);
        	}
      	}

        cost_L_LeftIndex = i;
        cost_R_LeftIndex = i;
        cost_I_LeftIndex = i;

        if(parent_i != NULL && cost1_L[parent_i_postL] == NULL) {
        	if (rowsToReuse_L.empty()) {
         		cost1_L[parent_i_postL] = new float[treeSizeB];
          		cost1_R[parent_i_postL] = new float[treeSizeB];
          		cost1_I[parent_i_postL] = new float[treeSizeB];
        	} else {
          		cost1_L[parent_i_postL] = rowsToReuse_L.top();
          		rowsToReuse_L.pop();
          		cost1_R[parent_i_postL] = rowsToReuse_R.top();
          		rowsToReuse_R.pop();
          		cost1_I[parent_i_postL] = rowsToReuse_I.top();
          		rowsToReuse_I.pop();
        	}
      	}

      	if (parent_i != NULL) {
        	cost_L_parent_i_LeftIndex = parent_i_postL;
        	cost_R_parent_i_LeftIndex = parent_i_postL;
        	cost_I_parent_i_LeftIndex = parent_i_postL;
        	strategy_parent_i_LeftIndex = parent_i_preL;
      	}

      	fill_n(cost2_L, treeSizeB, 0L);
      	fill_n(cost2_R, treeSizeB, 0L);
      	fill_n(cost2_I, treeSizeB, 0L);
      	fill_n(cost2_path, treeSizeB, 0);

      	for(int j = 0; j < treeSizeB; j++) {
 			int j_in_preL = (B_)->postL_to_preL[j];

 			if(DEBUG) {
 				ou << "compute " << to_string(i_in_preL) << ", " << to_string(j_in_preL) << endl;
 			}

 			strategyRightIndex = j_in_preL;
 			Node* parent_j = (*B_)[j_in_preL]->getParent();
 			int parent_j_preL, parent_j_postL;
        	if (parent_j != NULL) {
        		parent_j_preL = parent_j->getID();
          		parent_j_postL = (B_)->preL_to_postL[parent_j_preL];
        	}
        	if(DEBUG) {
        		ou << "parent i = "; 
        		if(parent_i == NULL) {
        			ou << "NULL";
        		} else {
        			ou << to_string(parent_i_preL);
        		}
        		ou << " parent j = ";
        		if(parent_j == NULL) {
        			ou << "NULL" << endl;
        		} else {
        			ou << to_string(parent_j_preL) << endl;
        		}
        	}
        	size_j = (*B_)[j_in_preL]->getSubTreeSize();
        	bool is_j_leaf = size_j == 1? true : false;
        	if (is_j_leaf) {
          		cost2_L[j] = 0L;
          		cost2_R[j] = 0L;
          		cost2_I[j] = 0L;
          		cost2_path[j] = j_in_preL;
        	}
        	minCost = 0x7fffffffffffffffL;
        	int pathLeaf = -1;
        	int treeToDecompose = 0;
        	float tmpCost = 0x7fffffffffffffffL;

        	if (size_i == 1) {
        		tmpCost = (float) (*B_)[j_in_preL]->getLeftmostForestNum();
        		if(DEBUG) {
					ou << "left leaf(j) in " << to_string(j_in_preL) << " = " << to_string((B_)->preL_to_lid[j_in_preL]) << " tmpCost = " << to_string(tmpCost) << endl;
				}
        		if(tmpCost < minCost) {
        			minCost = tmpCost;
        			pathLeaf = (B_)->preL_to_lid[j_in_preL];
        			treeToDecompose = 1;
        		}
          		tmpCost = (float) (*B_)[j_in_preL]->getRightmostForestNum();
          		if(DEBUG) {
					ou << "right leaf(j) in " << to_string(j_in_preL) << " = " << to_string((B_)->preL_to_rid[j_in_preL]) << " tmpCost = " << to_string(tmpCost) << endl;
				}
          		if(tmpCost < minCost) {
          			minCost = tmpCost;
          			pathLeaf = (B_)->preL_to_rid[j_in_preL];
          			treeToDecompose = 1;
          		}
        	}
        	if(size_j == 1) {
        		tmpCost = (float) (*A_)[i_in_preL]->getLeftmostForestNum();
        		if(DEBUG) {
					ou << "left leaf(i) in " << to_string(i_in_preL) << " = " << to_string(leftPath_i) << " tmpCost = " << to_string(tmpCost) << endl;
				}
        		if(tmpCost < minCost) {
        			minCost = tmpCost;
        			pathLeaf = leftPath_i;
        			treeToDecompose = 0;
        		}
        		tmpCost = (float) (*A_)[i_in_preL]->getRightmostForestNum();
        		if(DEBUG) {
					ou << "right leaf(i) in " << to_string(i_in_preL) << " = " << to_string(rightPath_i) << " tmpCost = " << to_string(tmpCost) << endl;
				}
        		if(tmpCost < minCost) {
        			minCost = tmpCost;
        			pathLeaf = rightPath_i;
        			treeToDecompose = 0;
        		}
        	}
        	if(size_i != 1) {
        		cost_L_RightIndex = j;//left path decomposition in i
				tmpCost = (float) size_i * (float) (*B_)[j_in_preL]->getLeftmostForestNum() + cost1_L[cost_L_LeftIndex][cost_L_RightIndex];
				if(DEBUG) {
					ou << "left leaf(i) in " << to_string(i_in_preL) << " = " << to_string(leftPath_i) << " tmpCost = " << to_string(tmpCost) << endl;
				}
          		if (tmpCost < minCost) {
            		minCost = tmpCost;
            		pathLeaf = leftPath_i;
            		treeToDecompose = 0;
            		//S[strategyLeftIndex][strategyRightIndex].setLeaf(leftPath_i);
            		//S[strategyLeftIndex][strategyRightIndex].setTreeToDecompose(0);
         	 	}

         	 	cost_R_RightIndex = j;//right path decomposition in i
         	 	tmpCost = (float) size_i * (float) (*B_)[j_in_preL]->getRightmostForestNum() + cost1_R[cost_R_LeftIndex][cost_R_RightIndex];
         	 	if(DEBUG) {
					ou << "right leaf(i) in " << to_string(i_in_preL) << " = " << to_string(rightPath_i) << " tmpCost = " << to_string(tmpCost) << endl;
				}
          		if (tmpCost < minCost) {
            		minCost = tmpCost;
            		pathLeaf = rightPath_i;
            		treeToDecompose = 0;
            		//S[strategyLeftIndex][strategyRightIndex].setLeaf(rightPath_i);
            		//S[strategyLeftIndex][strategyRightIndex].setTreeToDecompose(0);
          		}

          		cost_I_RightIndex = j;//special path decomposition in i
          		tmpCost = (float) size_i * (float) (*B_)[j_in_preL]->getSpecialForestNum() + cost1_I[cost_I_LeftIndex][cost_I_RightIndex];
          		if(DEBUG) {
					ou << "special leaf(i) in " << to_string(i_in_preL) << " = " << to_string((int)S[strategyLeftIndex][strategyRightIndex].getLeaf()) << " tmpCost = " << to_string(tmpCost) << " cost1_I[" << to_string(i_in_preL) << ", " << to_string(j_in_preL) << "] = " << to_string(cost1_I[cost_I_LeftIndex][cost_I_RightIndex]) << endl;
				}
          		if (tmpCost < minCost) {
            		minCost = tmpCost;
            		strategyRightIndex = j_in_preL;
            		pathLeaf = (int)S[strategyLeftIndex][strategyRightIndex].getLeaf();
            		treeToDecompose = 0;
            		//S[strategyLeftIndex][strategyRightIndex].setLeaf((int)S[strategyLeftIndex][strategyRightIndex].getLeaf() + 1);
            		//S[strategyLeftIndex][strategyRightIndex].setTreeToDecompose(0);
          		}
          	}
          	if(size_j != 1) {
          		//left path decomposition in j
          		tmpCost = (float) size_j * (float) i_leftmost_forest + cost2_L[j];
          		if(DEBUG) {
					ou << "left leaf(j) in " << to_string(j_in_preL) << " = " << to_string((B_)->preL_to_lid[j_in_preL]) << " tmpCost = " << to_string(tmpCost) << endl;
				}
          		if (tmpCost < minCost) {
            		minCost = tmpCost;
            		pathLeaf = (B_)->preL_to_lid[j_in_preL];
            		treeToDecompose = 1;
            		//S[strategyLeftIndex][strategyRightIndex].setLeaf((B_)->preL_to_lid[j_in_preL]);
            		//S[strategyLeftIndex][strategyRightIndex].setTreeToDecompose(1);
          		}

          		//right path decompostion in j
          		tmpCost = (float) size_j * (float) i_rightmost_forest + cost2_R[j];
          		if(DEBUG) {
					ou << "right leaf(j) in " << to_string(j_in_preL) << " = " << to_string((B_)->preL_to_rid[j_in_preL]) << " tmpCost = " << to_string(tmpCost) << endl;
				}
          		if (tmpCost < minCost) {
            		minCost = tmpCost;
            		pathLeaf = (B_)->preL_to_rid[j_in_preL];
            		treeToDecompose = 1;
            		//S[strategyLeftIndex][strategyRightIndex].setLeaf((B_)->preL_to_rid[j_in_preL]);
            		//S[strategyLeftIndex][strategyRightIndex].setTreeToDecompose(1);
          		}

          		//special path decompostion in j
          		tmpCost = (float) size_j * (float) i_special_forest + cost2_I[j];
          		if(DEBUG) {
					ou << "special leaf(j) in " << to_string(j_in_preL) << " = " << to_string(cost2_path[j]) << " tmpCost = " << to_string(tmpCost) << " cost2_I[" << to_string(j_in_preL) << "] = " << cost2_I[j] << endl;
				}
          		if (tmpCost < minCost) {
            		minCost = tmpCost;
            		pathLeaf = cost2_path[j];
            		treeToDecompose = 1;
            		//S[strategyLeftIndex][strategyRightIndex].setLeaf(cost2_path[j] + pathIDOffset + 1);
            		//S[strategyLeftIndex][strategyRightIndex].setTreeToDecompose(1);
          		}
        	}

        	if (parent_i != NULL) {
        		cost_R_parent_i_RightIndex = j;
        		cost1_R[cost_R_parent_i_LeftIndex][cost_R_parent_i_RightIndex] += minCost;
        		tmpCost = -minCost + cost1_I[i][j];
        		/*if(DEBUG) {
          			ou << "minCost = " << to_string(minCost) << " cost1_I[" << to_string(i_in_preL) << ", " << to_string(j_in_preL) << "] = " << to_string(cost1_I[i][j]) << endl;
          		}*/
        		
        		if (tmpCost < cost1_I[parent_i_postL][j]) {
        			cost_I_parent_i_RightIndex = j;
        			if(DEBUG) {
          				ou << "cost1_I[" << to_string(parent_i_preL) << ", " << to_string(parent_j_preL) << "] = " << to_string(tmpCost) << endl;
          			}
            		cost1_I[cost_I_parent_i_LeftIndex][cost_I_parent_i_RightIndex] = tmpCost;
            		strategy_parent_i_RightIndex = j_in_preL;
            		strategyRightIndex = j_in_preL;
            		S[strategy_parent_i_LeftIndex][strategy_parent_i_RightIndex] = S[strategyLeftIndex][strategyRightIndex];
            		/*if(DEBUG) {
            			ou << "S[" << to_string(strategy_parent_i_LeftIndex) << ", " << to_string(strategy_parent_i_RightIndex) << "] = S[" << to_string(strategyLeftIndex) << ", " << to_string(strategyRightIndex) << "] = " << to_string(S[strategyLeftIndex][strategyRightIndex].getLeaf()) << endl;
            		}*/
          		}

          		vector<Node*> children = parent_i->getChildren();
          		bool is_i_leftmost_child = children[0]->getID() == i_in_preL? true : false;
          		bool is_i_rightmost_child = children[children.size() - 1]->getID() == i_in_preL? true : false;
          		/*if(DEBUG) {
          			ou << "is_i_leftmost_child = " << to_string(is_i_leftmost_child) << endl;
          			ou << "is_i_rightmost_child = " << to_string(is_i_rightmost_child) << endl;
          		}*/
          		if (is_i_rightmost_child) {
          			cost_I_parent_i_RightIndex = j;
          			cost_R_parent_i_RightIndex = j;
          			if(DEBUG) {
          				ou << "cost1_I[" << to_string(parent_i_preL) << ", " << to_string(j_in_preL) << "](" << to_string(cost1_I[parent_i_postL][j]) << ") += ";
          			}
            		cost1_I[cost_I_parent_i_LeftIndex][cost_I_parent_i_RightIndex] += cost1_R[cost_R_parent_i_LeftIndex][cost_R_parent_i_RightIndex];
            		if(DEBUG) {				
          				ou << "cost1_R[" << to_string(i_in_preL) << ", " << to_string(j_in_preL) << "](" << to_string(cost1_R[parent_i_postL][j]) << ") = ";
          				ou << to_string(cost1_I[parent_i_postL][j]) << endl;
          			}
            		cost_R_parent_i_RightIndex = j;
            		cost_R_RightIndex = j;
            		cost1_R[cost_R_parent_i_LeftIndex][cost_R_parent_i_RightIndex] += cost1_R[cost_R_LeftIndex][cost_R_RightIndex] - minCost;
          		}
          		if (is_i_leftmost_child) {
          			cost_L_parent_i_RightIndex = j;
          			cost_L_RightIndex = j;
          			cost1_L[cost_L_parent_i_LeftIndex][cost_L_parent_i_RightIndex] += cost1_L[cost_L_LeftIndex][cost_L_RightIndex];
          		} else {
          			cost_L_parent_i_RightIndex = j;
          			cost1_L[cost_L_parent_i_LeftIndex][cost_L_parent_i_RightIndex] += minCost;
          		}
        	}


        	if (parent_j != NULL) {
          		cost2_R[parent_j_postL] += minCost;
          		tmpCost = -minCost + cost2_I[j];
          		/*if(DEBUG) {
          			ou << "minCost = " << to_string(minCost) << " cost2_I[" << to_string(j_in_preL) << "] = " << to_string(cost2_I[j]) << endl;
          		}
*/
          		if (tmpCost < cost2_I[parent_j_postL]) {
          			if(DEBUG) {
          				ou << "cost2_I[" << to_string(parent_j_preL) << "] = " << to_string(tmpCost) << endl;
          			}
            		cost2_I[parent_j_postL] = tmpCost;
            		cost2_path[parent_j_postL] = cost2_path[j];
          		}
          		vector<Node*> children = parent_j->getChildren();
          		bool is_j_leftmost_child = children[0]->getID() == j_in_preL? true : false;
          		bool is_j_rightmost_child = children[children.size() - 1]->getID() == j_in_preL? true : false;
          	/*	if(DEBUG) {
          			ou << "is_j_leftmost_child = " << to_string(is_j_leftmost_child) << endl;
          			ou << "is_j_rightmost_child = " << to_string(is_j_rightmost_child) << endl;
          		}*/
          		if (is_j_rightmost_child) {
          			if(DEBUG) {
          				ou << "cost2_I[" << to_string(parent_j_preL) << "](" << to_string(cost2_I[parent_j_postL]) << ") += "; 
          			}
            		cost2_I[parent_j_postL] += cost2_R[parent_j_postL];
            		if(DEBUG) {
          				ou << "cost2_R[" << to_string(parent_j_preL) << "](" << to_string(cost2_R[parent_j_postL]) << ") = ";
          				ou << to_string(cost2_I[parent_j_postL]) << endl;
          			}
            		cost2_R[parent_j_postL] += cost2_R[j] - minCost;
          		}
          		if (is_j_leftmost_child) {
          			/*if(DEBUG) {
          				ou << "cost2_L[" << to_string(parent_j_postL) << "] += cost2_L[" << to_string(j) << "] = " << cost2_L[j] << endl;
          				ou << "cost2_L[" << to_string(parent_j_postL) << "] = " << cost2_L[parent_j_postL] << endl;
          			}*/
            		cost2_L[parent_j_postL] += cost2_L[j];
          		} else {
          			/*if(DEBUG) {
          				ou << "cost2_L[" << to_string(parent_j_postL) << "] += "  << to_string(minCost) << endl;
          			}*/
            		cost2_L[parent_j_postL] += minCost;
          		}
        	}
        	if(DEBUG) {
        		ou << "S[" << to_string(strategyLeftIndex) << ", " << to_string(strategyRightIndex) << "] = " << to_string(pathLeaf) << endl;
        	}
        	S[strategyLeftIndex][strategyRightIndex].setLeaf(pathLeaf);
        	S[strategyLeftIndex][strategyRightIndex].setTreeToDecompose(treeToDecompose);
		}

        bool is_i_in_preL_leaf = (*A_)[i_in_preL]->getSubTreeSize() == 1? true : false;

        if (!is_i_in_preL_leaf) {
        	fill_n(cost1_L[i], treeSizeB, 0);
        	fill_n(cost1_R[i], treeSizeB, 0);
        	fill_n(cost1_I[i], treeSizeB, 0);
        	rowsToReuse_L.push(cost1_L[i]);
        	rowsToReuse_R.push(cost1_R[i]);
        	rowsToReuse_I.push(cost1_I[i]);
      	}
	}
	return S;
}

float TreeComparison::gted(Node* a, Node* b) {
	
	if(hasVisited[a->getID()][b->getID()] == true) return 0.0f;
	hasVisited[a->getID()][b->getID()] = true;
	int treeSizeA = a->getSubTreeSize();
	int treeSizeB = b->getSubTreeSize();
	if(DEBUG) {
		ou << "gted(" << to_string(a->getID()) << ", " << to_string(b->getID()) << ")" << endl;
		ou << "treeSizeA = " << to_string(treeSizeA) << endl;
		ou << "treeSizeB = " << to_string(treeSizeB) << endl;
	}
	
	if ((treeSizeA == 1 || treeSizeB == 1)) {
      //return spf1(a, treeSizeA, b, treeSizeB);
		if(DEBUG) {
			ou << "return 0.0f" << endl;
		}
		return 0.0f;
    }


	int pathLeaf = FreeStrategies[a->getID()][b->getID()].getLeaf();
	int treeToDecompose = FreeStrategies[a->getID()][b->getID()].getTreeToDecompose();
	Node* currentPathNode = treeToDecompose == 0? (*A_)[pathLeaf] : (*B_)[pathLeaf];


	if(treeToDecompose == 0) { // decompose tree A
		Node* parent = currentPathNode->getParent();
		int pathType = getPathType(A_, a, pathLeaf);// 0 left 1 right 2 special
		if(DEBUG) {
			ou << "getPathType A(" << to_string(a->getID()) << " ," << to_string(pathLeaf) << ") = " << to_string(pathType) << endl;
		}
		while(parent != NULL && parent->getID() >= a->getID()) {
        	vector<Node*> children = parent->getChildren();
        	for(int i = 0; i < children.size(); i++) {
          		Node* child = children[i];
          		if(DEBUG) {
          			ou << "A child = " << to_string(child->getID()) << " currentPathNode = " << to_string(currentPathNode->getID()) << " parent = " << to_string(parent->getID()) << endl;
          		}
          		if(child->getID() != currentPathNode->getID()) {
          			if(DEBUG) {
          				ou << "gted(" << to_string(a->getID()) << ", " << to_string(b->getID()) << ") ";
          				ou << "create problem in A " << "gted(" << to_string(child->getID()) << ", " << to_string(b->getID()) << ")" << endl;
          			}
            		gted(child, b);
          		}
        	}
        	currentPathNode = parent;
        	parent = currentPathNode->getParent();
        }

        if(DEBUG) {
      		ou << "swap = " << "false " << "pathType = " << to_string(pathType) << endl; 
      	}

/*      	if (pathType == 0) {
        	//return spfL(a, b, false);
        	return 0.0f;
      	}
      	else if (pathType == 1) {
        	//return spfR(a, b, false);
        	return 0.0f;
      	}*/

      	return spfA(a, b, pathLeaf, pathType, false);
	} 

	else if(treeToDecompose == 1) {
		Node* parent = currentPathNode->getParent();
		int pathType = getPathType(B_, b, pathLeaf);
		if(DEBUG) {
			ou << "getPathType B (" << to_string(b->getID()) << " ," << to_string(pathLeaf) << ") = " << to_string(pathType) << endl;
		}
		while(parent != NULL && parent->getID() >= b->getID()) {
			vector<Node*> children = parent->getChildren();
			for(int i = 0; i < children.size(); i++) {
				Node* child = children[i];
				if(DEBUG) {
          			ou << "A child = " << to_string(child->getID()) << " currentPathNode = " << to_string(currentPathNode->getID()) << " parent = " << to_string(parent->getID()) << endl;
          		}
				if(child->getID() != currentPathNode->getID()) {
					if(DEBUG) {
						ou << "gted(" << to_string(a->getID()) << ", " << to_string(b->getID()) << ") ";
          				ou << "create problem in B " << "gted(" << to_string(a->getID()) << ", " << to_string(child->getID()) << ")" << endl;
          			}
					gted(a, child);
				}
			}
        	currentPathNode = parent;
        	parent = currentPathNode->getParent();
		}

		if(DEBUG) {
      		ou << "swap = " << "true " << "pathType = " << to_string(pathType) << endl; 
      	}

		/*if(pathType == 0) {
			//return spfL(b, a, true);
			return 0.0f;
		}
		else if(pathType == 1) {
			//return spfR(b, a, true);
			return 0.0f;
		}*/
		return spfA(b, a, pathLeaf, pathType, true);
	}
};


float TreeComparison::spfL(Node* a, Node* b, int leaf, bool swap) {
	Tree *F, *G;
	if(swap) {
		F = B_;
		G = A_;
	} else {
		F = A_;
		G = B_;
	}
	int* keyRoots = new int[(*G)[b->getID()]->getSubTreeSize()];
	int firstKeyRoot = computeKeyRoots(G, b, leaf, keyRoots, 0);	

	float** forestdist = new float*[(*F)[a->getID()]->getSubTreeSize() + 1];
	for(int i = 0; i < (*F)[a->getID()]->getSubTreeSize() + 1; i++) {
		forestdist[i] = new float[(*G)[b->getID()]->getSubTreeSize() + 1];
	}


}

int TreeComparison::computeKeyRoots(Tree* G, Node* b, int leaf, int* keyRoots, int index) {
	
	keyRoots[index++] = b->getID();

	int pathNode = leaf;
	while(pathNode > b->getID()) {
		Node* parent = (*G)[b->getID()]->getParent();
		vector<Node*> children;
		if(parent != NULL)  children = parent->getChildren();
		for(int i = 0; i < children.size(); i++) {
			if(children[i]->getID() != pathNode) {
				computeKeyRoots(G, children[i], G->preL_to_lid[children[i]->getID()], keyRoots, index);
			}
		}
		pathNode = parent->getID();
	}
	return index;
}

float TreeComparison::spfA(Node* a, Node* b, int leaf, int pathType, bool swap) {
	Tree *F, *G;
	if(swap) {
		F = B_;
		G = A_;
	} else {
		F = A_;
		G = B_;
	}
	int endF = a->getID(); 
	int endG = b->getID();
	int sizeF = a->getSubTreeSize();
	int sizeG = b->getSubTreeSize();
	int endF_in_preR = F->preL_to_preR[endF];
	int endG_in_preR = G->preL_to_preR[endG];
	int endPathNode = leaf;
  int endPathNode_in_preR = F->preL_to_preR[endPathNode];
	int startPathNode = -1;
	int lFFirst, lFLast, lF;
	int rFFirst, rFLast, rF;
	int lGFirst, lGLast;
	int rGFirst, rGLast;

	
	//loop A
	while(endPathNode >= endF) {
		endPathNode_in_preR = F->preL_to_preR[endPathNode];
		int startPathNode_in_preR = startPathNode == -1? 0x7fffffff : F->preL_to_preR[startPathNode];

		int parent_of_endPathNode_preL = (*F)[endPathNode]->getParent() == NULL? 0x7fffffff : (*F)[endPathNode]->getParent()->getID();
		int parent_of_endPathNode_preR = (*F)[endPathNode]->getParent() == NULL? 0x7fffffff : F->preL_to_preR[parent_of_endPathNode_preL];

		bool hasLeftPart;
		bool hasRightPart;

		int lF_prev = endPathNode;

		if(startPathNode - endPathNode > 1) {
			hasLeftPart = true;
		} else {
			hasLeftPart = false;
		}
		if(startPathNode >= 0 && startPathNode_in_preR - endPathNode_in_preR > 1) {
			hasRightPart = true;
		} else {
			hasRightPart = false;
		}

    if(DEBUG) {
      ou << "spfA(" << to_string(a->getID()) << ", " << to_string(b->getID()) << ") " << swap << " ";
      ou << "pathType: " << to_string(pathType) << " hasLeftPart: " << to_string(hasLeftPart) << " hasRightPart: " << to_string(hasRightPart) << endl;
    }


    // right path left decomposition
		if(pathType == 1 || pathType == 2 && hasLeftPart) {
			// lFFirst and LFLast is important in this condition.
			// rGFirst and rGLast is to set in this stage.
			if(startPathNode == -1) {
				lFFirst = endPathNode;//the first node is the node on the path
				rFFirst = endPathNode_in_preR;// the first node is the node on the path
			} else {
				lFFirst = startPathNode - 1;//the first node is the node to the left of the path
				rFFirst = startPathNode_in_preR;//rFFirst set to the node on the path
			}
			if(!hasRightPart) {
				rFLast = endPathNode_in_preR;
			}
			lFLast = hasRightPart? endPathNode + 1 : endPathNode;

			rGLast = G->preL_to_preR[endG];
			rGFirst = (rGLast + sizeG) - 1; // get the leftmost child in G

			fn[fn_ft_length - 1] = -1;
			/*if(DEBUG) {
				ou << "initial fn and ft endG = " << to_string(endG) << " endG + sizeG = " << to_string(endG + sizeG) << endl;
			}*/
      for (int i = endG; i < endG + sizeG; i++) {
        fn[i] = -1;
        ft[i] = -1;
      }

			//loop B
			for(int rG = rGFirst; rG >= rGLast; rG--) {
				if(DEBUG) {
					ou << "new Round B" << endl;
				}
				int rG_in_preL = (G)->preR_to_preL[rG];
				Node* parent = (*G)[rG_in_preL]->getParent();
				int parent_of_rG_in_preL = parent == NULL? 0x7fffffff : parent->getID();
				int parent_of_rG_in_preR = parent == NULL? 0x7fffffff : G->preL_to_preR[parent_of_rG_in_preL];
				lGFirst = G->preR_to_preL[rG];// lGFirst is set to rGFirst;
				
				int rGminus1_in_preL = rG <= endG_in_preR? 0x7fffffff : G->preR_to_preL[rG - 1];// rG should greater than endG_in_preR cause rG is the inner node of subtree enG
				int rGminus1_in_preR = rG <= endG_in_preR? 0x7fffffff : rG - 1;
				
				if (pathType == 1){
          if (lGFirst == endG || rGminus1_in_preL != parent_of_rG_in_preL) {// parent not exist or not the rightmost child
            lGLast = lGFirst;//lGLast is set to lGFirst
          } else {
            lGLast = parent_of_rG_in_preL + 1;//lGLast is set to the leftmost child of rG's parent
          }
        } else {
          lGLast = lGFirst == endG? lGFirst : endG + 1;//lGLast is set to the leftmost child of the whole tree
        }
			
        updateFnArray(G->preL_to_ln[lGFirst], lGFirst, endG); //stores the counter in D loop fn[ln] stores the start point
        updateFtArray(G->preL_to_ln[lGFirst], lGFirst); 
				
        //loop C
				for(int lF = lFFirst; lF >= lFLast; lF--) {
					if(DEBUG) {
						ou << "new round C hasLeftPart = " << to_string(hasLeftPart) << endl;
					}
          rF = startPathNode_in_preR;
          if (lF == lFLast && !hasRightPart) {
              rF = rFLast;
          }
					int lG = lGFirst;
					int lF_in_preR = (F)->preL_to_preR[lF];

					if(DEBUG) {
          	ou << "Left (" << to_string(lF) << ", " << to_string(rF) << ", " << to_string(lG) << ", " << to_string(rG) << ")" << endl;
          	ou << "Save to S[" << to_string(lF) << ", " << to_string(lG) << "]" << endl;
					}

					int GcurrentForestSize = (*G)[lG]->getSubTreeSize();
          int GcurrentForestCost = (swap ? (G)->preL_to_sumDelCost[lG] : (G)->preL_to_sumInsCost[lG]); 

					bool fForestIsTree = lF_in_preR == rF;
					int lFSubtreeSize = (*F)[lF]->getSubTreeSize();
					bool lFIsLeftSiblingOfCurrentPathNode = lF + lFSubtreeSize == startPathNode;
					bool lFIsConsecutiveNodeOfCurrentPathNode = startPathNode - lF == 1;

					int case1SLeftIndex, case1SRightIndex;//S[lF + 1, lG];
          int case1TLeftIndex, case1TRightIndex;//T[lG, rG];
          			
          int case2SLeftIndex, case2SRightIndex;//S[lF, lG];

          int case3SLeftIndex, case3SRightIndex;
          int case3TLeftIndex, case3TRightIndex;

          case1SLeftIndex =  lF + 1;//fixed
          case2SLeftIndex = lF;//fixed

          case1TRightIndex = rG;//fixed

          case3TRightIndex = rG;//fixed

          float case1 = 0, case2 = 0, case3 = 0;
          int case1_case, case2_case, case3_case;

          float minCost = 0;

          case1_case = 1;
          case3_case = 1;

          if (fForestIsTree) { // F_{lF,rF} is a tree.
            if (lFSubtreeSize == 1) { // F_{lF,rF} is a single node.
              // F_{lF, rF} - lF = null
              case1_case = 3;
            } else if (lFIsConsecutiveNodeOfCurrentPathNode) { // F_{lF,rF}-lF is the path node subtree.
              // F_{lF, rF} - rF = tree
              case1_case = 2;
            }
            
            case3 = 0;//F_{lF, rF} - F(lF) = null
            if(DEBUG) {
          		ou << "case3 = 0" << endl; 
          	}
            
            case3_case = 2;
          } else {
            if (lFIsConsecutiveNodeOfCurrentPathNode) {
              // F_{lF, rF} - lF = tree
              case1_case = 2;
            }
            //case3 = FcurrentForestCost - (swap ? (A_)->preL_to_sumInsCost[lF] : (A_)->preL_to_sumDelCost[lF]); // USE COST MODEL - Delete F_{lF,rF}-F_lF.
            if(DEBUG) {
              ou << "case3_case FcurrentForest - F(lF)" << endl;
            }
              			
        		if (lFIsLeftSiblingOfCurrentPathNode) {
              case3_case = 3;
            }
          }

          if(case3_case == 1) {
            case3SLeftIndex = lF + lFSubtreeSize;
          }

          switch(case1_case) {
            case 1: 
             	case1SRightIndex = lG;
              //case1 = s[case1SLeftIndex][case1SRightIndex]; 
              if(DEBUG) {
             		ou << "case1_case1 s[" << to_string(case1SLeftIndex) << ", " << to_string(case1SRightIndex) << "]" << endl;
             	}
             	break;
            
            case 2: 
              case1TLeftIndex = lG;
              //case1 = t[case1TLeftIndex][case1TRightIndex]; 
              if(DEBUG) {
              	ou << "case1_case2 t[" << to_string(case1TLeftIndex) << ", " << to_string(case1TRightIndex) << "]" << endl; 
              }
              break;
            
            case 3: 
              //case1 = GcurrentForestCost; 
              if(DEBUG) {
              	ou << "case1_case3 " << to_string(GcurrentForestCost) << endl; 
              }
              break; // USE COST MODEL - Insert G_{lG,rG}.
          }

          case1 += (swap ? costModel_.ins((*F)[lF]->getLabel()) : costModel_.del((*F)[lF]->getLabel()));
          //minCost = case1;

          if (GcurrentForestSize == 1) { // G_{lG,rG} is a single node.
            //case2 = FcurrentForestCost; // USE COST MODEL - Delete F_{lF,rF}.
            if(DEBUG) {
              ou << "case2_case1 FcurrentForestCost" << endl;
            }
          } else { // G_{lG,rG} is a tree.
            //case2 = q[lF];
            if(DEBUG) {
            	ou << "case2_case2 q[" << to_string(lF) << "]" << endl;
            }
          }

          if (minCost < case3) {
            //case3 += swap? d[lG][lF] : d[lF][lG];
            if(DEBUG) {
            	if(swap) ou << "case3_case3 d[" << to_string(lG) << ", " << to_string(lF) << "]" << endl;
            	else ou << "case3_case3 d[" << to_string(lF) << ", " << to_string(lG) << "]" << endl;
            }
            if(minCost < case3) {
            	case3 += swap? costModel_.ren((*G)[lG]->getLabel(), (*F)[lF]->getLabel()) : costModel_.ren((*F)[lF]->getLabel(), (*G)[lG]->getLabel());
            } 
            if(minCost < case3) {
            	minCost = case3;
            }
          }
					lG = ft[lG];
					
          //loop D
					while (lG >= lGLast) {
						if(DEBUG) {
							ou << "Left (" << to_string(lF) << ", " << to_string(rF) << ", " << to_string(lG) << ", " << to_string(rG) << ")" << endl;
							ou << "Save to s[" << to_string(lF) << ", " << to_string(lG) << "]" << endl;
						}

						GcurrentForestSize++;
						GcurrentForestCost += (swap ? costModel_.del((*G)[lG]->getLabel()) : costModel_.ins((*G)[lG]->getLabel()));
						minCost = 0;

						switch(case1_case) {
              case 1:
                case1SRightIndex = lG;
                //case1 = s[case1SLeftIndex][case1SRightIndex] + (swap? costModel_.ins((*A_)[lF]->getLabel()) : costModel_.del((*A_)[lF]->getLabel())); 
                if(DEBUG) {
                	ou << "case1_case1 s[" << to_string(case1SLeftIndex) << ", " << to_string(case1SRightIndex) << "]" << endl;
                }
                break; // USE COST MODEL - Delete lF, leftmost root node in F_{lF,rF}.
              
              case 2: 
                case1TLeftIndex = lG;
                if(DEBUG) {
                	ou << "case1_case2 t[" << to_string(case1TLeftIndex) << ", " << to_string(case1TRightIndex) << "]" << endl;
                }
                //case1 = t[case1TLeftIndex][case1TRightIndex] + (swap ? costModel_.ins((*A_)[lF]->getLabel()) : costModel_.del((*A_)[lF]->getLabel())); 
                break; // USE COST MODEL - Delete lF, leftmost root node in F_{lF,rF}.
              
              case 3: 
                //case1 = GcurrentForestCost + (swap? costModel_ins((*A_)[lF]->getLabel()) : costModel_.del((*A_)[lF]->getLabel()));
                if(DEBUG) {
                	ou << "case1_case3 " << to_string(GcurrentForestCost) << endl;
                }
                break; // USE COST MODEL - Insert G_{lG,rG} and elete lF, leftmost root node in F_{lF,rF}.
            }
            //minCost = case1;

            case2SRightIndex = fn[lG];
            //case2 = s[case2SLeftIndex][case2SRightIndex] + (swap ? costModel_.del((*B_)[lG]->getLabel()) : costModel_.ins((*B_)[lG]->getLabel()));
            if(DEBUG) {
              ou << "case2 s[" << to_string(case2SLeftIndex) << ", " << to_string(case2SRightIndex) << "]" << endl;
            }
            
            if(case2 < minCost) {
              //minCost = case2;
            } 

            //case3 = swap ? d[lG][lF] : d[lF][lG];
            if(DEBUG) {
              if(swap) {
              	ou << "case3 d[" << to_string(lG) << ", " << to_string(lF) << "]" << endl;
              } else {
              	ou << "case3 d[" << to_string(lF) << ", " << to_string(lG) << "]" << endl;
              }
            }
            
            //if (minCost < case3) {
              switch(case3_case) {
                case 1: 
                  case3SRightIndex = fn[lG] + (*G)[lG]->getSubTreeSize();
                  if(DEBUG) {
                    ou << "case3 s[" << to_string(case3SLeftIndex) << ", " << to_string(case3SRightIndex) << "]" << endl;
                  }
                  //case3 += s[case3SLeftIndex][case3SRightIndex]; 
                  break;
                
                case 2: 
                  //case3 += GcurrentForestCost - (swap ? (B_)->preL_to_sumDelCost[lG] : (B_)->preL_to_sumInsCost[lG]); 
                  if(DEBUG) {
                    ou << "case3 " << "GcurrentForestCost - G(lG)" << endl;
                  }
                  break; // USE COST MODEL - Insert G_{lG,rG}-G_lG.
                
                case 3: 
                  case3TLeftIndex = fn[lG + (*G)[lG]->getSubTreeSize() - 1];
                	if(DEBUG) {
                		ou << "case3 t[" << to_string(case3TLeftIndex) << ", " << to_string(case3TRightIndex) << "]" << endl;
                	}
                  //case3 += t[case3TLeftIndex][case3TRightIndex]; 
                  break;
              }
              
              if (case3 < minCost) {
                case3 += (swap ? costModel_.ren((*G)[lG]->getLabel(), (*F)[lF]->getLabel()) : costModel_.ren((*F)[lF]->getLabel(), (*G)[lG]->getLabel())); // USE COST MODEL - Rename the leftmost root nodes in F_{lF,rF} and G_{lG,rG}.
                if (case3 < minCost) {
                  minCost = case3;
                }
              }
            //}
						lG = ft[lG];
					}
					lF_prev = lF;
				}
				if(DEBUG) {
					ou << "rGminus1_in_preR = " << to_string(rGminus1_in_preR) << " rG = " << to_string(rG) << " parent_of_rG_in_preL = " << to_string(parent_of_rG_in_preL) << " parent_of_rG_in_preR = " << to_string(parent_of_rG_in_preR) << endl;
				}
				if(rGminus1_in_preR == parent_of_rG_in_preR && rGminus1_in_preR != 0x7fffffff) {
					if (!hasRightPart) {
            if (hasLeftPart) {
              if(swap) {
              	if(DEBUG) {
              		ou << "save to d[" << to_string(parent_of_rG_in_preL) << ", " << to_string(endPathNode) << "] = " << "s[" << to_string(lFLast + 1) << ", " << to_string(rGminus1_in_preL + 1) << "]" << endl;
              	}
              } else {
              	if(DEBUG) {
              		ou << "save to d[" << to_string(endPathNode) << ", " << to_string(parent_of_rG_in_preL) << "] = " << "s[" << to_string(lFLast + 1) << ", " << to_string(rGminus1_in_preL + 1) << "]" << endl;
              	}
              }
            }

            if (endPathNode > 0 && endPathNode == parent_of_endPathNode_preL + 1 && endPathNode_in_preR == parent_of_endPathNode_preR + 1) {
              if (swap) {
                if(DEBUG) {
                  ou << "save to d[" << to_string(parent_of_rG_in_preL) << ", " << to_string(parent_of_endPathNode_preL) << "] = " << "s[" << to_string(lFLast) << ", " << to_string(rGminus1_in_preL + 1) << "]" << endl;
                }
              } else {
                if(DEBUG) {
                  ou << "save to d[" << to_string(parent_of_endPathNode_preL) << ", " << to_string(parent_of_rG_in_preL) << "] = " << "s[" << to_string(lFLast) << ", " << to_string(rGminus1_in_preL + 1) << "]" << endl;
                }
              }
            }
          }

          for (int lF = lFFirst; lF >= lFLast; lF--) {
            if(DEBUG) {
              ou << "q[" << to_string(lF) << "] = " << "s[" << to_string(lF) << ", " << to_string(parent_of_rG_in_preL + 1) << "]" << endl;
            }
          }
			  }

			  for (int lG = lGFirst; lG >= lGLast; lG = ft[lG]) {
				  if(DEBUG) {
					 ou << "t[" << to_string(lG) << ", " << to_string(rG) << "] = " << "s[" << to_string(lFLast) << ", " << to_string(lG) << "]" << endl;
				  }
        }
		  }
	  }

    // left path right decomposit
	  if (pathType == 0 || pathType == 2 && hasRightPart || pathType == 2 && !hasLeftPart && !hasRightPart) {
		  if (startPathNode == -1) {
        lFFirst = endPathNode;
        rFFirst = F->preL_to_preR[endPathNode];
      } else {
        rFFirst = F->preL_to_preR[startPathNode] - 1;//the node right to the node on the path
        lFFirst = endPathNode + 1;//lFirst is set to the node on the path
      }

      lFLast = endPathNode;
      rFLast = F->preL_to_preR[endPathNode];

      lGLast = endG;
      lGFirst = (lGLast + sizeG) - 1;

      fn[fn_ft_length - 1] = -1;
  	  if(DEBUG) {
			   ou << "initial fn and ft endG_in_preR = " << to_string(endG_in_preR) << " endG_in_preR + sizeG = " << to_string(endG_in_preR + sizeG) << endl;
		  }
      
      for (int i = endG_in_preR; i < endG_in_preR + sizeG; i++) {
        fn[i] = -1;
        ft[i] = -1;
      }

      //loop B'
      for (int lG = lGFirst; lG >= lGLast; lG--) {
        if(DEBUG) {
				  ou << "new Round B'" << endl;
			  }
        Node* parent = (*G)[lG]->getParent();
        int parent_of_lG_in_preL = parent == NULL? 0x7fffffff: parent->getID();
        int parent_of_lG_in_preR = parent == NULL? 0x7fffffff : G->preL_to_preR[parent->getID()];// not exist -1;
			  rGFirst = G->preL_to_preR[lG];
			  int lG_in_preR = G->preL_to_preR[lG];

			  int lGminus1_in_preL = lG <= endG? 0x7fffffff : lG - 1;
			  int lGminus1_in_preR = lG <= endG? 0x7fffffff : G->preL_to_preR[lG - 1];

			  if (pathType == 0) {
          if (lG == endG || lGminus1_in_preL != parent_of_lG_in_preL) {//parent not exists or not the leftmost child.
            rGLast = rGFirst;
          } else {
            rGLast = parent_of_lG_in_preR + 1;
          }
        } else {// left and right
          rGLast = rGFirst == endG_in_preR ? rGFirst : endG_in_preR;
        }

			/*if(DEBUG) {
				ou << "updateFnArray(" << to_string(G->preR_to_ln[rGFirst]) << ", " << to_string(rGFirst) << ", " << to_string(endG_in_preR) << ")" << endl;
			}*/
			  updateFnArray(G->preR_to_ln[rGFirst], rGFirst, endG_in_preR);
			
			/*if(DEBUG) {
				ou << "updateFtArray(" << to_string(G->preR_to_ln[rGFirst]) << ", " << to_string(rGFirst) << ")" << endl;
			}*/
        updateFtArray(G->preR_to_ln[rGFirst], rGFirst);
          	
      /*if(DEBUG) {
       ou << "endG_in_preR = " << to_string(endG_in_preR) << endl;
       ou << "start from rG = " << to_string(rGFirst) << endl;
       ou << "FN" << endl;
      for(int i = endG; i < endG + sizeG + 1; i++) {
        ou << fn[i] << " ";
      }
      ou << endl;
      ou << "FT" << endl;
      for(int i = endG; i < endG + sizeG + 1; i++) {
        ou << ft[i] << " ";
      }
      ou << endl;
      }*/
        lF = lF_prev;
      // loop C'
        for(int rF = rFFirst; rF >= rFLast; rF--) {
          if(DEBUG) {
            ou << "new Round C'" << endl;
          }
          lF = startPathNode;
          if (rF == rFLast) {
              lF = lFLast;
          }
          int rG = rGFirst;
          int rG_in_preL = (G)->preR_to_preL[rG];
          			
          if(rF == rFLast) lF = F->preR_to_preL[rFLast]; 
          	
          if(DEBUG) {
            ou << "Right (" << to_string(lF) << ", " << to_string(rF) << ", " << to_string(lG) << ", " << to_string(rG) << ")" << endl;
            ou << "Save to S[" << to_string(rF) << ", " << to_string(rG) << "]" << endl;
				  }

          int rF_in_preL = (F)->preR_to_preL[rF];
          			
          bool FForestIsTree = lF == rF_in_preL;
          int rFSubtreeSize = (*F)[rF_in_preL]->getSubTreeSize();
          			  			
          int case1SLeftIndex, case1SRightIndex;//S[rF + 1, rG];
          int case1TLeftIndex, case1TRightIndex;//T[lG, rG];

          int case2SLeftIndex, case2SRightIndex;//S[rF, rG];

          int case3SLeftIndex, case3SRightIndex;
          int case3TLeftIndex, case3TRightIndex;
        
          float case1 = 0, case2 = 0, case3 = 0;
        	int case1_case, case2_case, case3_case;

         	int GcurrentForestSize = (*G)[lG]->getSubTreeSize();
          float GcurrentForestCost = (swap ? (G)->preL_to_sumDelCost[lG] : (G)->preL_to_sumInsCost[lG]); // USE COST MODEL - reset to subtree insertion cost.

          float minCost = 0;


          case1SLeftIndex = rF + 1;//fixed
          case1SRightIndex = rG;

          case1TLeftIndex = lG;//fixed
          case1TRightIndex = rG;

          case2SLeftIndex = rF;//fixed

          case3TLeftIndex = lG;//fixed


          bool rFIsConsecutiveNodeOfCurrentPathNode;
          bool rFIsRightSiblingOfCurrentPathNode;

          case1_case = 1;
          case3_case = 1;//otherwise

          if (startPathNode > 0) {
            rFIsConsecutiveNodeOfCurrentPathNode = startPathNode_in_preR - rF == 1;
            rFIsRightSiblingOfCurrentPathNode = rF + rFSubtreeSize == startPathNode_in_preR;
          } else {
            rFIsConsecutiveNodeOfCurrentPathNode = false;//consecutiveNode use T;
            rFIsRightSiblingOfCurrentPathNode = false;//
          }

          if(FForestIsTree) {
            if(rFSubtreeSize == 1) {
          	  // F_{lF, rF} - rF = null
          	  //case1 = GcurrentForestCost;//sumG
          	  case1_case = 3;
            } else if(rFIsConsecutiveNodeOfCurrentPathNode) {
          	  // F_{lF, rF} - rF = tree
          	  //case1 = t[case1TLeftIndex][case1TRightIndex];//T[lG, rG];
          	  case1_case = 2;
            }
            case3 = 0;
            if(DEBUG) {
          	  ou << "case3 = 0" << endl; 
            }
            case3_case = 2;// F_{lF, rF} - F(rF) = null
            } else {
              if (rFIsConsecutiveNodeOfCurrentPathNode) {// F_{lF, rF} - rF = the subforest to the left of the path
          	   //case1 = t[case1TLeftIndex][case1TRightIndex];//T[lG, rG]
          	   case1_case = 2;
              } else {//otherwise
          	    //case1 = s[case1SLeftIndex][case1SRightIndex];//S[rF + 1, rG];// have calculate
          	    case1_case = 1;
              }
              //case3 = FcurrentForestCost - (swap ? (A_)->preL_to_sumInsCost[rF_in_preL] : (A_)->preL_to_sumDelCost[rF_in_preL]);// the first case in G should be G_{lG, rG} - l(rG) = null // F_{lF, rF} - F(rF), G_{lG, rG} - G(rG)
              if(DEBUG) {
                ou << "case3_case FcurrentForest - F(rF)" << endl;
              }
              if (rFIsRightSiblingOfCurrentPathNode) {
          	    case3_case = 3; // use T
              }
            }

            if (case3_case == 1) {
              case3SLeftIndex = rF + rFSubtreeSize;//delete the whole rightmost tree//otherwise
            }
          
            switch(case1_case) {
              case 1:
                //case1 = s[case1SLeftIndex][case1SRightIndex];
                if(DEBUG) {
            	    ou << "case1_case1 = s[" << to_string(case1SLeftIndex) << ", " << to_string(case1SRightIndex) << "]" << endl; 
                }
              break;
          
              case 2:
                //case1 = t[case1TLeftIndex][case1TRightIndex];
                if(DEBUG) {
            	    ou << "case1_case2 = t[" << to_string(case1TLeftIndex) << ", " << to_string(case1TRightIndex) << "]" << endl;
                } 
                break;
              case 3:
                //case1 = GcurrentForestCost;
                if(DEBUG) {
            	    ou << "case1_case3 = " << GcurrentForestCost << endl;
                }
                break;  				
            }
            //case1 += (swap ? costModel_.ins((*A_)[rF]->getLabel()) : costModel_.del((*A_)[rF]->getLabel()));
            minCost = case1;

            if (GcurrentForestSize == 1) {// the first case in G should be a node or a tree
              //case2 = FcurrentForestCost;
              if(DEBUG) {
                ou << "case2_case1 = FcurrentForestCost" << endl;
              }
            } else {
              //case2 = q[rF];
              if(DEBUG) {
                ou << "case2_case2 = q[" << to_string(rF) << "]" << endl;
              }
            }

            //case2 += (swap ? costModel_.del((*B_)[rG_in_preL]->getLabel()) : costModel_.ins((*B_)[rG_in_preL]->getLabel()));
            if(case2 < minCost) {
              minCost = case2;
            }
        
            //if(minCost < case3) { 
            //case3 += swap ? d[rG_in_preL][rF_in_preL] : d[rF_in_preL][rG_in_preL];// F(rF) - rF
              if(DEBUG) {
                if(swap) ou << "case3_case3 d[" << to_string(rG_in_preL) << ", " << to_string(rF_in_preL) << "]" << endl;
                else ou << "case3_case3 d[" << to_string(rF_in_preL) << ", " << to_string(rG_in_preL) << "]" << endl;
              }
          
              if(minCost < case3) {
                case3 += (swap ? costModel_.ren((*G)[rG_in_preL]->getLabel(), (*F)[rF_in_preL]->getLabel()) : costModel_.ren((*F)[rF_in_preL]->getLabel(), (*G)[rG_in_preL]->getLabel()));
              }
          
              if(case3 < minCost) {
                minCost = case3;
              }
            //}

        s[rF][rG] = minCost;
        rG = ft[rG];	
        
        // loop D'
        while(rG >= rGLast) {// every G is a subforest not a subtree
          rG_in_preL = (G)->preR_to_preL[rG];

        	if(DEBUG) {
          	ou << "Right (" << to_string(lF) << ", " << to_string(rF) << ", " << to_string(lG) << ", " << to_string(rG) << ")" << endl;
          	ou << "Save to s[" << to_string(rF) << ", " << to_string(rG) << "]" << endl;
					}

					GcurrentForestSize++;
					GcurrentForestCost += (swap ? costModel_.del((*G)[rG_in_preL]->getLabel()) : costModel_.ins((*G)[rG_in_preL]->getLabel()));

					switch (case1_case) {
            case 1:
              //case1 = s[case1SLeftIndex][case1SRightIndex] + (swap ? costModel_.ins((*A_)[rF]->getLabel()) : costModel_.del((*A_)[rF]->getLabel())); 
              case1SRightIndex = rG;
              if(DEBUG) {
                ou << "case1_case1 s[" << to_string(case1SLeftIndex) << ", " << to_string(case1SRightIndex) << "]" << endl; 
              } 
              break; 
            case 2: 
              case1TRightIndex = rG;
              //case1 = t[case1TLeftIndex][case1TRightIndex] + (swap ? costModel_.ins((*A_)[rF]->getLabel()) : costModel_.del((*A_)[rF]->getLabel())); 
              if(DEBUG) {
                ou << "case1_case2 t[" << to_string(case1TLeftIndex) << ", " << to_string(case1TRightIndex) << "]" << endl;
              }
              break; 
            case 3: 
              //case1 = GcurrentForestCost + (swap ? costModel_.ins((*A_)[rF]->getLabel()) : costModel_.del((*A_)[rF]->getLabel())); 
              if(DEBUG) {
                ou << "case1_case3 " << GcurrentForestCost << endl;
              }
              break; 
          }
          
          minCost = case1;

          case2SRightIndex = fn[rG];
          if(DEBUG) {
            ou << "case2_case3 s[" << to_string(case2SLeftIndex) << ", " << to_string(case2SRightIndex) << "]" << endl;
          }
          
          case2 = s[case2SLeftIndex][case2SRightIndex] + (swap ? costModel_.del((*G)[rG_in_preL]->getLabel()) : costModel_.ins((*G)[rG_in_preL]->getLabel()));//G is not a tree or a node for sure in D loop
          if(case2 < minCost) {
            minCost = case2;
          }

          //case3 = swap ? d[rG_in_preL][rF_in_preL] : d[rF_in_preL][rG_in_preL];//F_{rF} - rF, G_{rG} - rG
          if(DEBUG) {
            if(swap) {
              ou << "case3_case d[" << to_string(rG_in_preL) << ", " << to_string(rF_in_preL) << "]" << endl;
            } else {
              ou << "case3_case d[" << to_string(rF_in_preL) << ", " << to_string(rG_in_preL) << "]" << endl;
            }
          }
          
          //if(case3 < minCost) {
            switch(case3_case) {
              case 1: 
              	case3SRightIndex = fn[(rG + (*G)[rG_in_preL]->getSubTreeSize()) - 1];
              	//case3 += s[case3SLeftIndex][case3SRightIndex];
              	if(DEBUG) {
              	  ou << "case3_case1 s[" << to_string(case3SLeftIndex) << ", " << to_string(case3SRightIndex) << "]" << endl; 
              	}
              	break;
              
              case 2: 
              	//case3 += GcurrentForestCost - (swap ? (B_)->preL_to_sumDelCost[rG_in_preL] : (B_)-?preL_to_sumInsCost[rG_in_preL]);
              	if(DEBUG) {
              		ou << "case3_case2 " << "GcurrentForestCost - G(rG) " << endl; 
              	}
              	break;
              
              case 3: 
              	case3TRightIndex = fn[(rG + (*G)[rG_in_preL]->getSubTreeSize()) - 1];
              	//case3 += t[case3TLeftIndex][case3TRightIndex];
              	if(DEBUG) {
              		ou << "case3_case3 t[" << to_string(case3TLeftIndex) << ", " << to_string(case3TRightIndex) << "]" << endl; 
              	}
              	break;
            }
              
            if(minCost < case3) {
              case3 += (swap ? costModel_.ren((*G)[rG_in_preL]->getLabel(), (*F)[rF_in_preL]->getLabel()) : costModel_.ren((*F)[rF_in_preL]->getLabel(), (*G)[rG_in_preL]->getLabel()));
              if(case3 < minCost) {
              	minCost = case3;
              }
            }
          //}

            s[rF][rG] = minCost;
            rG = ft[rG];
          }
        }

        if(lGminus1_in_preL == parent_of_lG_in_preL && lGminus1_in_preL != 0x7fffffff) { // lG is the leftmost child of its parent
    
          if(hasRightPart) {
            if(swap) {
              if(DEBUG) {
          	   ou << "save to d[" << to_string(parent_of_lG_in_preL) << ", " << to_string(endPathNode) << "] = " << "s[" << to_string(rFLast) << ", " << to_string(lGminus1_in_preR + 1) << "]" << endl; //rightmosts child of p(lG)
              }
            } else {
              if(DEBUG) {
                ou << "save to d[" << to_string(endPathNode) << ", " << to_string(parent_of_lG_in_preL) << "] = " << "s[" << to_string(rFLast) << ", " << to_string(lGminus1_in_preR + 1) << "]" << endl; //rightmosts child of p(lG)
              }
            }
          }
      
          if (endPathNode > 0 && endPathNode == parent_of_endPathNode_preL + 1 && endPathNode_in_preR == parent_of_endPathNode_preR + 1) {//no left and right
            if(swap) {
              if(DEBUG) {
            	 ou << "save to d[" << to_string(parent_of_lG_in_preL) << ", " << to_string(parent_of_endPathNode_preL) << "] = " << "s[" << to_string(rFLast) << ", " << to_string(lGminus1_in_preR + 1) << "]" << endl; //rightmosts child of p(lG)
              }
            } else {
              if(DEBUG) {
          	    ou << "save to d[" << to_string(parent_of_endPathNode_preL) << ", " << to_string(parent_of_lG_in_preL) << "] = " << "s[" << to_string(rFLast) << ", " << to_string(lGminus1_in_preR + 1) << "]" << endl; //rightmosts child of p(lG)
              }
            }
          }

          for (int rF = rFFirst; rF >= rFLast; rF--) {
            if(DEBUG) {
              ou << "q[" << to_string(rF) << "] = " << "s[" << to_string(rF) << ", " << to_string(parent_of_lG_in_preR + 1) << "]" << endl;	
            }
          }
        }
        for (int rG = rGFirst; rG >= rGLast; rG = ft[rG]) {
          if(DEBUG) {
            ou << "t[" << to_string(lG) << ", " << to_string(rG) << "] = " << "s[" << to_string(rFLast) << ", " << to_string(rG) << "]" << endl;
          }
        }
      }
    }
    rF = endPathNode_in_preR;//in D' loop
    startPathNode = endPathNode;
    endPathNode = (*F)[endPathNode] ->getParent() == NULL? -1 : (*F)[endPathNode] ->getParent()->getID();	
    endPathNode_in_preR = F->preL_to_preR[endPathNode];
  }
};

void TreeComparison::updateFnArray(int lnForNode, int node, int currentSubtreePreL) {
    if (lnForNode >= currentSubtreePreL) {
      fn[node] = fn[lnForNode];//the last leaf node whose next leaf is lnForNode
      fn[lnForNode] = node;// fn[lnfornode] points to the start point
     /* if(DEBUG) {
      	ou << "fn[" << to_string(node) << "] = fn[" << to_string(lnForNode) << "] = " << to_string(fn[lnForNode]) << endl; 
      	ou << "fn[" << to_string(lnForNode) << "] = " << to_string(node) << endl;
      }*/
    } else {
      fn[node] = fn[fn_ft_length - 1];
      fn[fn_ft_length - 1] = node;
      /*if(DEBUG) {
      	ou << "O fn[" << to_string(node) << "] = fn[" << to_string(fn_ft_length - 1) << "] = " << fn[fn_ft_length - 1] << endl; 
      	ou << "O fn[" << to_string(fn_ft_length - 1) << "] = " << to_string(node) << endl;
      }*/
    }
}

void TreeComparison::updateFtArray(int lnForNode, int node) {
    ft[node] = lnForNode;
    /*if(DEBUG) {
    	ou << "ft[" << to_string(node) << "] = " << to_string(lnForNode) << endl;
    }*/
    if(fn[node] > -1) {
      ft[fn[node]] = node;
     /* if(DEBUG) {
      	ou << "ft[fn[" << to_string(node) << "]] = " << to_string(node) << endl;  
      }*/
    }
}


int TreeComparison::getPathType(Tree* tree, Node* node, int leaf) {
	if(tree->preL_to_lid[node->getID()] == leaf) return 0;
	else if(tree->preL_to_rid[node->getID()] == leaf) return 1;
	else return 2;
}

void TreeComparison::strategyComputation() {
	vector<Node*> preA = A_->getPreL();
	vector<Node*> preB = B_->getPreL();

	computeSumInsAndDelCost(A_);
	computeSumInsAndDelCost(B_);
	deltaInit();

	free(preA[0], preB[0]);
	Strategy** S = APTED_ComputeOptStrategy_postL();
	if(DEBUG) {
		ou << "RESULT" << endl;
	/*	for(int i = 0; i < treeSizeA; i++) {
			for(int j = 0; j < treeSizeB; j++) {
				ou << Free[i][j] << " ";
			}
			ou << endl;
		}*/

		for(int i = 0; i < treeSizeA; i++) {
			for(int j = 0; j < treeSizeB; j++) {
				if(&FreeStrategies[i][j] != NULL) {
					ou << FreeStrategies[i][j].getLeaf() << " in ";
					if(FreeStrategies[i][j].getTreeToDecompose() == 0) ou << "A ";
					else ou << "B ";
				}
			}
			ou << endl;
		}

		ou << "RESULT 2" << endl;

		for(int i = 0; i < treeSizeA; i++) {
			for(int j = 0; j < treeSizeB; j++) {
				if(&S[i][j] != NULL) {
					ou << S[i][j].getLeaf() << " in ";
					if(S[i][j].getTreeToDecompose() == 0) ou << "A ";
					else ou << "B ";
				}
			}
			ou << endl;
		}

	}
	gted(preA[0], preB[0]);

};


int TreeComparison::free(Node* a, Node* b) {
	/*if(DEBUG) {
		ou << "Compute Free(" << to_string(a->getID()) << ", " << to_string(b->getID()) << ")" << endl;
	}*/
/*	if(Free[a->getID()][b->getID()] != -1) {
		if(DEBUG) {
			ou << "Free(" << to_string(a->getID()) << ", " << to_string(b->getID()) << ") = " << to_string(Free[a->getID()][b->getID()]) << endl;
		}
		return Free[a->getID()][b->getID()];
	}*/
	
	/*if(DEBUG) {
		ou << a->toString() << endl;
		ou << b->toString() << endl;
	}*/
	if(Free[a->getID()][b->getID()] != -1) return Free[a->getID()][b->getID()];
	vector<Node*> childrenA = a->getChildren();
	vector<Node*> childrenB = b->getChildren();
	Strategy freeS;
	int min = INT_MAX;
	int freeSumA = 0;
	vector<int> childrenSizeSumA;
	int freeSumB = 0;
	vector<int> childrenSizeSumB;

	if(childrenA.empty()) {
		int left = b->getRightmostForestNum();
		int right = b->getLeftmostForestNum();
		if(min > right) {
			min = right;
			freeS.setDirection(0);
			freeS.setTreeToDecompose(0);
			freeS.setKeyNode(a->getID());
			freeS.setLeaf(a->getID());
		} else if(min > left){
			min = left;
			freeS.setDirection(1);
			freeS.setTreeToDecompose(0);
			freeS.setKeyNode(a->getID());
			freeS.setLeaf(a->getID());
		}
		if(DEBUG) {
			ou << "Compute Free(" << to_string(a->getID()) << ", " << to_string(b->getID()) << ")" << endl;
			ou << "If select " << to_string(a->getID()) << " in Tree A #Subproblem: " << to_string(min) << " Direction: ";
			if(left > right) ou << "Right" << endl;
			else ou << "Left" << endl;
		}
	}
	if(childrenB.empty()) {
		int left = a->getRightmostForestNum();
		int right = a->getLeftmostForestNum();
		if(min > right) {
			min = right;
			freeS.setDirection(0);
			freeS.setTreeToDecompose(1);
			freeS.setKeyNode(b->getID());
			freeS.setLeaf(b->getID());
		} else if(min > left){
			min = left;
			freeS.setDirection(1);
			freeS.setTreeToDecompose(1);
			freeS.setKeyNode(b->getID());
			freeS.setLeaf(b->getID());
		}

		if(DEBUG) {
			ou << "Compute Free(" << to_string(a->getID()) << ", " << to_string(b->getID()) << ")" << endl;
			ou << "If select " << to_string(b->getID()) << " in Tree B #Subproblem: " << to_string(min) << " Direction: ";
			if(left > right) ou << "Right" << endl;
			else ou << "Left" << endl;
		}
	} 

	int prev = 0;
	if(!childrenA.empty()) {
		for(int i = 0; i < childrenA.size(); i++) {
			freeSumA += free(childrenA[i], b);
			prev += childrenA[i]->getSubTreeSize();
			childrenSizeSumA.push_back(prev);
		}
	}
	prev = 0;
	if(!childrenB.empty()) {
		for(int i = 0; i < childrenB.size(); i++) {
			freeSumB += free(a, childrenB[i]);
			prev += childrenB[i]->getSubTreeSize();
			childrenSizeSumB.push_back(prev);
		}
	}

	if(!childrenA.empty()) {
		int aleftmost = freeSumA - free(childrenA[0], b) + leftA(childrenA[0], b) + b->getLeftmostForestNum() * (a->getSubTreeSize() - childrenA[0]->getSubTreeSize());
		if(DEBUG) {
			ou << "Compute Free(" << to_string(a->getID()) << ", " << to_string(b->getID()) << ")" << endl;
			ou << "If select " << to_string(childrenA[0]->getID()) << "(leftmost) in Tree A #Subproblem: " << to_string(aleftmost) << " Direction: Right" << endl;
		}
		if(min > aleftmost) {
			min = aleftmost;
			freeS.setKeyNode(childrenA[0]->getID());
			freeS.setTreeToDecompose(0);
			freeS.setDirection(0);
			freeS.setLeaf(LeftAStrategies[childrenA[0]->getID()][b->getID()].getLeaf());
		}
		for(int i = 1; i < childrenA.size() - 1; i++) {
			int prefix = freeSumA - free(childrenA[i], b) + allA(childrenA[i], b);
			//int left = b->getRightmostForestNum() * (1 + childrenSizeSumA[i - 1])  + b->getSpecialForestNum() * (childrenSizeSumA[childrenA.size() - 1] - childrenSizeSumA[i]);
			//int right = b->getLeftmostForestNum() * (1 + childrenSizeSumA[childrenA.size() - 1] - childrenSizeSumA[i]) + b->getSpecialForestNum() * (childrenSizeSumA[i - 1]);
			int sum = prefix + b->getSpecialForestNum() * (a->getSubTreeSize() - childrenA[i]->getSubTreeSize());
			//if(left > right) sum = prefix + right;
			//else sum = prefix + left;
			if(min > sum) {
				min = sum;
				freeS.setKeyNode(childrenA[i]->getID());
				freeS.setTreeToDecompose(0);
				freeS.setLeaf(AllAStrategies[childrenA[i]->getID()][b->getID()].getLeaf());
				//if(left > right)freeS.setDirection(0);
				//else freeS.setDirection(1);
			}
			if(DEBUG) {
				ou << "Compute Free(" << to_string(a->getID()) << ", " << to_string(b->getID()) << ")" << endl;
				ou << "If select " << to_string(childrenA[i]->getID()) << " in Tree A #Subproblem: " << to_string(sum) << " Direction: ";
				if(left > right) ou << "Right" << endl;
				else ou << "Left" << endl;
			}
		}
		int arightmost = freeSumA - free(childrenA[childrenA.size() - 1], b) + rightA(childrenA[childrenA.size() - 1], b) + b->getRightmostForestNum() * (a->getSubTreeSize() - childrenA[childrenA.size() - 1]->getSubTreeSize());
		if(DEBUG) {
			ou << "Compute Free(" << to_string(a->getID()) << ", " << to_string(b->getID()) << ")" << endl;
			ou << "If select " << to_string(childrenA[childrenA.size() - 1]->getID()) << "(rightmost) in Tree A #Subproblem: " << to_string(arightmost) << " Direction: Left" << endl;
		}
		if(min > arightmost) {
			min = arightmost;
			freeS.setKeyNode(childrenA[childrenA.size() - 1]->getID());
			freeS.setLeaf(RightAStrategies[childrenA[childrenA.size() - 1]->getID()][b->getID()].getLeaf());
			freeS.setTreeToDecompose(0);
			freeS.setDirection(1);
		}
	}

	if(!childrenB.empty()) {
		int bleftmost = freeSumB - free(a, childrenB[0]) + leftB(a, childrenB[0]) + a->getLeftmostForestNum() * (b->getSubTreeSize() - childrenB[0]->getSubTreeSize());
		if(DEBUG) {
			ou << "Compute Free(" << to_string(a->getID()) << ", " << to_string(b->getID()) << ")" << endl;
			ou << "If select " << to_string(childrenB[0]->getID()) << "(leftmost) in Tree B #Subproblem: " << to_string(bleftmost) << " Direction: Right" << endl;
		}
		if(min > bleftmost) {
			min = bleftmost;
			freeS.setKeyNode(childrenB[0]->getID());
			freeS.setLeaf(LeftBStrategies[a->getID()][childrenB[0]->getID()].getLeaf());
			freeS.setTreeToDecompose(1);
			freeS.setDirection(0);
		}

		for(int i = 1; i < childrenB.size() - 1; i++) {
			int prefix = freeSumB - free(a, childrenB[i]) + allB(a, childrenB[i]);
			//int left = a->getRightmostForestNum() * (1 + childrenSizeSumB[i - 1]) + a->getSpecialForestNum() * (childrenSizeSumB[childrenB.size() - 1] - childrenSizeSumB[i]);
			//int right = a->getLeftmostForestNum() * (1 + childrenSizeSumB[childrenB.size() - 1] - childrenSizeSumB[i]) + a->getSpecialForestNum() * (childrenSizeSumB[i - 1]);
			int sum = prefix + a->getSpecialForestNum() * (b->getSubTreeSize() - childrenB[i]->getSubTreeSize()
				);
			//if(left > right) sum = prefix + right;
			//else sum = prefix + left;
			if(min > sum) {
				min = sum;
				freeS.setKeyNode(childrenB[i]->getID());
				freeS.setLeaf(AllBStrategies[a->getID()][childrenB[i]->getID()].getLeaf());
				freeS.setTreeToDecompose(1);
				//if(left > right)freeS.setDirection(0);
				//else freeS.setDirection(1);
			}
			if(DEBUG) {
				ou << "Compute Free(" << to_string(a->getID()) << ", " << to_string(b->getID()) << ")" << endl;
				ou << "If select " << to_string(childrenB[i]->getID()) << " in Tree B #Subproblem: " << to_string(sum) << " Direction: ";
				if(left > right) ou << "Right" << endl;
				else ou << "Left" << endl;
			}
		}

		int brightmost = freeSumB - free(a, childrenB[childrenB.size() - 1]) + rightB(a, childrenB[childrenB.size() - 1]) + a->getRightmostForestNum() * (b->getSubTreeSize() - childrenB[childrenB.size() - 1]->getSubTreeSize());
		if(DEBUG) {
			ou << "Compute Free(" << to_string(a->getID()) << ", " << to_string(b->getID()) << ")" << endl;
			ou << "If select " << to_string(childrenB[childrenB.size() - 1]->getID()) << "(rightmost) in Tree B #Subproblem: " << to_string(brightmost) << " Direction: Left" << endl;
		}
		if(min > brightmost) {
			min = brightmost;
			freeS.setKeyNode(childrenB[childrenB.size() - 1]->getID());
			freeS.setLeaf(RightBStrategies[a->getID()][childrenB[childrenB.size() - 1]->getID()].getLeaf());
			freeS.setTreeToDecompose(1);
			freeS.setDirection(1);
		}
	}
	Free[a->getID()][b->getID()] = min;
	FreeStrategies[a->getID()][b->getID()] = freeS;
	if(DEBUG) {
		ou << "FreeS(" << to_string(a->getID()) << ", " << to_string(b->getID()) << ")" << endl;
		ou << FreeStrategies[a->getID()][b->getID()].toString() << endl;
	}
	return min;
};

int TreeComparison::leftA(Node* a, Node* b) {
	/*if(DEBUG) {
		ou << "Compute LeftA(" << to_string(a->getID()) << ", " << to_string(b->getID()) << ")" << endl;
	}*/
	/*if(LeftA[a->getID()][b->getID()] != -1) {
		if(DEBUG) {
			ou << "Compute LeftA(" << to_string(a->getID()) << ", " << to_string(b->getID()) << ") = " << to_string(LeftA[a->getID()][b->getID()]) << endl;
		}
		return LeftA[a->getID()][b->getID()];
	}*/
	if(LeftA[a->getID()][b->getID()] != -1) return LeftA[a->getID()][b->getID()];
	vector<Node*> childrenA = a->getChildren();
	int min = INT_MAX;
	Strategy leftAS;
	if(childrenA.empty()) {
		min = b->getLeftmostForestNum();
		leftAS.setKeyNode(a->getID());
		leftAS.setLeaf(a->getID());
		leftAS.setTreeToDecompose(0);
		leftAS.setDirection(0);
	} else {
		int freeSumA = 0;
		vector<int> childrenSizeSumA;
		int prev = 0;
		for(int i = 0; i < childrenA.size(); i++) {
			freeSumA += free(childrenA[i], b);
			prev += childrenA[i]->getSubTreeSize();
			childrenSizeSumA.push_back(prev);
		}
		int aleftmost = freeSumA - free(childrenA[0], b) + leftA(childrenA[0], b) + b->getLeftmostForestNum() * (a->getSubTreeSize() - childrenA[0]->getSubTreeSize());
		if(DEBUG) {
			ou << "aleftmost = " << freeSumA - free(childrenA[0], b) + leftA(childrenA[0], b) << " + " << b->getLeftmostForestNum() * (a->getSubTreeSize() - childrenA[0]->getSubTreeSize()) << " = " << aleftmost << endl;
		}
		if(min >aleftmost) {
			min = aleftmost;
			leftAS.setKeyNode(childrenA[0]->getID());
			leftAS.setLeaf(LeftAStrategies[childrenA[0]->getID()][b->getID()].getLeaf());
			leftAS.setTreeToDecompose(0);
			leftAS.setDirection(0);
		}

		for(int i = 1; i < childrenA.size() - 1; i++) {
			int prefix = freeSumA - free(childrenA[i], b) + allA(childrenA[i], b);
			int left = b->getSpecialForestNum() * (a->getSubTreeSize() - childrenA[i]->getSubTreeSize());
			int right = b->getLeftmostForestNum() * (1 + childrenSizeSumA[childrenSizeSumA.size() - 1] - childrenSizeSumA[i]) + b->getSpecialForestNum() * (childrenSizeSumA[i - 1]);
			int sum = 0;
			if(left > right) sum = prefix + right;
			else sum = prefix + left;
			if(min > sum) {
				min = sum;
				leftAS.setKeyNode(childrenA[i]->getID());
				leftAS.setTreeToDecompose(0);
				leftAS.setLeaf(AllAStrategies[childrenA[i]->getID()][b->getID()].getLeaf());
				if(left > right)leftAS.setDirection(0);
				else leftAS.setDirection(1);
			}
		}
		int arightmost = freeSumA - free(childrenA[childrenA.size() - 1], b) + allA(childrenA[childrenA.size() - 1], b) + b->getSpecialForestNum() * (a->getSubTreeSize() - childrenA[childrenA.size() - 1]->getSubTreeSize());
		if(min > arightmost) {
			min = arightmost;
			leftAS.setKeyNode(childrenA[childrenA.size() - 1]->getID());
			leftAS.setLeaf(AllAStrategies[childrenA[childrenA.size() - 1]->getID()][b->getID()].getLeaf());
			leftAS.setTreeToDecompose(0);
			leftAS.setDirection(1);
		}
	}
	if(DEBUG) {
		ou << "LeftA[" << to_string(a->getID()) << ", " << to_string(b->getID()) << "] = " << to_string(min) << endl;
	}
	LeftA[a->getID()][b->getID()] = min;
	LeftAStrategies[a->getID()][b->getID()] = leftAS;
	return min;

};

int TreeComparison::rightA(Node* a, Node* b) {

	if(RightA[a->getID()][b->getID()] != -1) return RightA[a->getID()][b->getID()];
	vector<Node*> childrenA = a->getChildren();
	int min = INT_MAX;
	Strategy rightAS;
	if(childrenA.empty()) {
		min = b->getRightmostForestNum();
		rightAS.setKeyNode(a->getID());
		rightAS.setLeaf(a->getID());
		rightAS.setTreeToDecompose(0);
		rightAS.setDirection(1);
	} else {
		int freeSumA = 0;
		vector<int> childrenSizeSumA;
		int prev = 0;
		for(int i = 0; i < childrenA.size(); i++) {
			freeSumA += free(childrenA[i], b);
			prev += childrenA[i]->getSubTreeSize();
			childrenSizeSumA.push_back(prev);
		}
		int aleftmost = freeSumA - free(childrenA[0], b) + allA(childrenA[0], b) + b->getSpecialForestNum() * (a->getSubTreeSize() - childrenA[0]->getSubTreeSize());
		if(min >aleftmost) {
			min = aleftmost;
			rightAS.setKeyNode(childrenA[0]->getID());
			rightAS.setLeaf(AllAStrategies[childrenA[0]->getID()][b->getID()].getLeaf());
			rightAS.setTreeToDecompose(0);
			rightAS.setDirection(0);
		}

		for(int i = 1; i < childrenA.size() - 1; i++) {
			int prefix = freeSumA - free(childrenA[i], b) + allA(childrenA[i], b);
			int left = b->getRightmostForestNum() * (1 + childrenSizeSumA[i - 1]) + b->getSpecialForestNum() * (childrenSizeSumA[childrenA.size() - 1] - childrenSizeSumA[i]);
			int right = b->getSpecialForestNum() * (a->getSubTreeSize() - childrenA[i]->getSubTreeSize());
			int sum = 0;
			if(left > right) sum = prefix + right;
			else sum = prefix + left;
			if(min > sum) {
				min = sum;
				rightAS.setKeyNode(childrenA[i]->getID());
				rightAS.setLeaf(AllAStrategies[childrenA[i]->getID()][b->getID()].getLeaf());
				rightAS.setTreeToDecompose(0);
				if(left > right)rightAS.setDirection(0);
				else rightAS.setDirection(1);
			}
		}
		int arightmost = freeSumA - free(childrenA[childrenA.size() - 1], b) + rightA(childrenA[childrenA.size() - 1], b) + b->getRightmostForestNum()* (a->getSubTreeSize() - childrenA[childrenA.size() - 1]->getSubTreeSize());
		if(min >= arightmost) {
			min = arightmost;
			rightAS.setKeyNode(childrenA[childrenA.size() - 1]->getID());
			rightAS.setLeaf(RightAStrategies[childrenA[childrenA.size() - 1]->getID()][b->getID()].getLeaf());
			rightAS.setTreeToDecompose(0);
			rightAS.setDirection(1);
		}
	}
	RightA[a->getID()][b->getID()] = min;
	RightAStrategies[a->getID()][b->getID()] = rightAS;
	if(DEBUG) {
		ou << "RightA[" << to_string(a->getID()) << ", " << to_string(b->getID()) << "] set to " << to_string(rightAS.getLeaf()) << endl;
	}
	return min;
};

int TreeComparison::allA(Node* a, Node* b) {
	if(AllA[a->getID()][b->getID()] != -1) return AllA[a->getID()][b->getID()];
	vector<Node*> childrenA = a->getChildren();
	int min = INT_MAX;
	Strategy allAS;
	if(childrenA.empty()) {
		min = b->getSpecialForestNum();
		allAS.setKeyNode(a->getID());
		allAS.setLeaf(a->getID());
		allAS.setTreeToDecompose(0);
	} else {
		int freeSumA = 0;
		vector<int> childrenSizeSumA;
		int prev = 0;
		for(int i = 0; i < childrenA.size(); i++) {
			freeSumA += free(childrenA[i], b);
			prev += childrenA[i]->getSubTreeSize();
			childrenSizeSumA.push_back(prev);
		}
		int aleftmost = freeSumA - free(childrenA[0], b) + allA(childrenA[0], b) + b->getSpecialForestNum() * (a->getSubTreeSize() - childrenA[0]->getSubTreeSize());
		if(min >aleftmost) {
			min = aleftmost;
			allAS.setKeyNode(childrenA[0]->getID());
			allAS.setLeaf(AllAStrategies[childrenA[0]->getID()][b->getID()].getLeaf());
			allAS.setTreeToDecompose(0);
		}

		for(int i = 1; i < childrenA.size(); i++) {
			int sum = freeSumA - free(childrenA[i], b) + allA(childrenA[i], b) + b->getSpecialForestNum() * (a->getSubTreeSize() - childrenA[i]->getSubTreeSize());
			if(min > sum) {
				min = sum;
				allAS.setKeyNode(childrenA[i]->getID());
				allAS.setTreeToDecompose(0);
				allAS.setLeaf(AllAStrategies[childrenA[i]->getID()][b->getID()].getLeaf());
				if(left > right)allAS.setDirection(0);
				else allAS.setDirection(1);
			}
		}
	}
	AllA[a->getID()][b->getID()] = min;
	AllAStrategies[a->getID()][b->getID()] = allAS;
	return min;
};

int TreeComparison::leftB(Node* a, Node* b) {
	if(LeftB[a->getID()][b->getID()] != -1) return LeftB[a->getID()][b->getID()];
	vector<Node*> childrenB = b->getChildren();
	int min = INT_MAX;
	Strategy leftBS;
	if(childrenB.empty()) {
		min = a->getLeftmostForestNum();
		leftBS.setKeyNode(b->getID());
		leftBS.setLeaf(b->getID());
		leftBS.setTreeToDecompose(1);
		leftBS.setDirection(0);
	} else {
		int freeSumB = 0;
		vector<int> childrenSizeSumB;
		int prev = 0;
		for(int i = 0; i < childrenB.size(); i++) {
			freeSumB += free(a, childrenB[i]);
			prev += childrenB[i]->getSubTreeSize();
			childrenSizeSumB.push_back(prev);
		}
		int bleftmost = freeSumB - free(a, childrenB[0]) + leftB(a, childrenB[0]) + a->getLeftmostForestNum() * (b->getSubTreeSize() - childrenB[0]->getSubTreeSize());
		if(min >bleftmost) {
			min = bleftmost;
			leftBS.setKeyNode(childrenB[0]->getID());
			leftBS.setLeaf(LeftBStrategies[a->getID()][childrenB[0]->getID()].getLeaf());
			leftBS.setTreeToDecompose(1);
			leftBS.setDirection(0);
		}

		for(int i = 1; i < childrenB.size() - 1; i++) {
			int prefix = freeSumB - free(a, childrenB[i]) + allB(a, childrenB[i]);
			int left = a->getSpecialForestNum() * (b->getSubTreeSize() - childrenB[i]->getSubTreeSize());
			int right = a->getLeftmostForestNum() * (1 + childrenSizeSumB[childrenSizeSumB.size() - 1] - childrenSizeSumB[i]) + a->getSpecialForestNum() * (childrenSizeSumB[i - 1]);
			int sum;
			if(left > right) sum = prefix + right;
			else sum = prefix + left;
			if(min > sum) {
				min = sum;
				leftBS.setKeyNode(childrenB[i]->getID());
				leftBS.setLeaf(AllBStrategies[a->getID()][childrenB[i]->getID()].getLeaf());
				leftBS.setTreeToDecompose(1);
				if(left > right)leftBS.setDirection(0);
				else leftBS.setDirection(1);
			}
		}
		int brightmost = freeSumB - free(a, childrenB[childrenB.size() - 1]) + allB(a, childrenB[childrenB.size() - 1]) + a->getSpecialForestNum() * (b->getSubTreeSize() - childrenB[childrenB.size() - 1]->getSubTreeSize());
		if(min > brightmost) {
			min = brightmost;
			leftBS.setKeyNode(childrenB[childrenB.size() - 1]->getID());
			leftBS.setLeaf(AllBStrategies[a->getID()][childrenB[childrenB.size() - 1]->getID()].getLeaf());
			leftBS.setTreeToDecompose(1);
			leftBS.setDirection(1);
		}
	}
	if(DEBUG) {
		ou << "LeftB[" << to_string(a->getID()) << ", " << to_string(b->getID()) << "] = " << to_string(min) << endl;
	}
	LeftB[a->getID()][b->getID()] = min;
	LeftBStrategies[a->getID()][b->getID()] = leftBS;
	return min;
};

int TreeComparison::rightB(Node* a, Node* b) {
	if(RightB[a->getID()][b->getID()] != -1) return RightB[a->getID()][b->getID()];
	vector<Node*> childrenB = b->getChildren();
	int min = INT_MAX;
	Strategy rightBS;
	if(childrenB.empty()) {
		min = a->getRightmostForestNum();
		rightBS.setKeyNode(b->getID());
		rightBS.setLeaf(b->getID());
		rightBS.setTreeToDecompose(1);
		rightBS.setDirection(1);
	} else {
		int freeSumB = 0;
		vector<int> childrenSizeSumB;
		int prev = 0;
		for(int i = 0; i < childrenB.size(); i++) {
			freeSumB += free(a, childrenB[i]);
			prev += childrenB[i]->getSubTreeSize();
			childrenSizeSumB.push_back(prev);
		}
		int bleftmost = freeSumB - free(a, childrenB[0]) + allB(a, childrenB[0]) + a->getSpecialForestNum() * (b->getSubTreeSize() - childrenB[0]->getSubTreeSize());
		if(DEBUG) {
			ou << "bleftmost = " << bleftmost << endl;
		}
		if(min > bleftmost) {
			min = bleftmost;
			rightBS.setKeyNode(childrenB[0]->getID());
			rightBS.setLeaf(AllBStrategies[a->getID()][childrenB[0]->getID()].getLeaf());
			rightBS.setTreeToDecompose(1);
			rightBS.setDirection(0);
		}

		for(int i = 1; i < childrenB.size() - 1; i++) {
			int prefix = freeSumB - free(a, childrenB[i]) + allB(a, childrenB[i]);
			int left = a->getRightmostForestNum() * (1 + childrenSizeSumB[i - 1]) + a->getSpecialForestNum() * (childrenSizeSumB[childrenB.size() - 1] - childrenSizeSumB[i]);
			int right = a->getSpecialForestNum() * (b->getSubTreeSize() - childrenB[i]->getSubTreeSize());
			int sum = 0;
			if(left > right) sum = prefix + right;
			else sum = prefix + left;
			if(min > sum) {
				min = sum;
				rightBS.setKeyNode(childrenB[i]->getID());
				rightBS.setLeaf(AllBStrategies[a->getID()][childrenB[i]->getID()].getLeaf());
				rightBS.setTreeToDecompose(1);
				if(left > right)rightBS.setDirection(0);
				else rightBS.setDirection(1);
			}
			if(DEBUG) {
				ou << "childrenB " << childrenB[i]->getID() << " = " << sum << endl; 
			}
		}
		int brightmost = freeSumB - free(a, childrenB[childrenB.size() - 1]) + rightB(a, childrenB[childrenB.size() - 1]) + a->getRightmostForestNum()* (b->getSubTreeSize() - childrenB[childrenB.size() - 1]->getSubTreeSize());
		if(DEBUG) {
			ou << "brightmost = " << freeSumB - free(a, childrenB[childrenB.size() - 1]) + rightB(a, childrenB[childrenB.size() - 1]) << " + " << a->getRightmostForestNum()* (b->getSubTreeSize() - childrenB[childrenB.size() - 1]->getSubTreeSize()) << " = " << brightmost << endl;
		}
		if(min >= brightmost) {
			min = brightmost;
			rightBS.setKeyNode(childrenB[childrenB.size() - 1]->getID());
			rightBS.setLeaf(RightBStrategies[a->getID()][childrenB[childrenB.size() - 1]->getID()].getLeaf());
			rightBS.setTreeToDecompose(1);
			rightBS.setDirection(1);
		}
	}
	RightB[a->getID()][b->getID()] = min;
	RightBStrategies[a->getID()][b->getID()] = rightBS;
	if(DEBUG) {
		ou << "RightB(" << to_string(a->getID()) << ", " << to_string(b->getID()) << ") = " << to_string(min) << endl;
	}
	return min;
};

int TreeComparison::allB(Node* a, Node* b) {
	if(AllB[a->getID()][b->getID()] != -1) return AllB[a->getID()][b->getID()];
	vector<Node*> childrenB = b->getChildren();
	int min = INT_MAX;
	Strategy allBS;
	if(childrenB.empty()) {
		min = a->getSpecialForestNum();
		allBS.setLeaf(b->getID());
		allBS.setKeyNode(b->getID());
		allBS.setTreeToDecompose(1);
	} else {
		int freeSumB = 0;
		vector<int> childrenSizeSumB;
		int prev = 0;
		for(int i = 0; i < childrenB.size(); i++) {
			freeSumB += free(a, childrenB[i]);
			prev += childrenB[i]->getSubTreeSize();
			childrenSizeSumB.push_back(prev);
		}
		int bleftmost = freeSumB - free(a, childrenB[0]) + allB(a, childrenB[0]) + a->getSpecialForestNum() * (b->getSubTreeSize() - childrenB[0]->getSubTreeSize());
		if(min >bleftmost) {
			min = bleftmost;
			allBS.setKeyNode(childrenB[0]->getID());
			allBS.setLeaf(AllBStrategies[a->getID()][childrenB[0]->getID()].getLeaf());
			allBS.setTreeToDecompose(1);
		}

		for(int i = 1; i < childrenB.size(); i++) {
			int sum = freeSumB - free(a, childrenB[i]) + allB(a, childrenB[i]) + a->getSpecialForestNum() * (b->getSubTreeSize() - childrenB[i]->getSubTreeSize());
			if(min > sum) {
				min = sum;
				allBS.setKeyNode(childrenB[i]->getID());
				allBS.setLeaf(AllBStrategies[a->getID()][childrenB[i]->getID()].getLeaf());
				allBS.setTreeToDecompose(1);
				if(left > right)allBS.setDirection(0);
				else allBS.setDirection(1);
			}
		}
	}
	AllB[a->getID()][b->getID()] = min;
	AllBStrategies[a->getID()][b->getID()] = allBS;
	return min;
};