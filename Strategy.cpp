#include "Strategy.h"

Strategy::Strategy() {

};

void Strategy::setKeyNode(int keynode) {
	keynode_ = keynode;
};
int Strategy::getKeyNode(void) const {
	return keynode_;
};

void Strategy::setTreeToDecompose(int treeToDecompose) {
	treeToDecompose_ = treeToDecompose;
};
int Strategy::getTreeToDecompose(void) const {
	return treeToDecompose_;
};

void Strategy::setDirection(int direction) {
	direction_ = direction;
};
int Strategy::getDirection(void) const {
	return direction_;
};

string Strategy::toString() const {
	string res;
	res += "Keynode: " + to_string(keynode_);
	if(treeToDecompose_ == 0) res += " in Tree A\n";
	else res += " in TreeB\n";
	res += "Direction: ";
	if(direction_ == 0) res += "right";
	else res += "left";
	return res;
};
