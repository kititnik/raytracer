#include "bvh_node.hpp"

bool BVHNode::isLeaf() const {
    if(right_node_ == nullptr && left_node_ == nullptr) {
        return true;
    }
    return false;
}

const Rectangle& BVHNode::GetBoundingBox() const {
    return bounding_box_;
}
