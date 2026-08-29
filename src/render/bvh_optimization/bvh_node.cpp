#include "bvh_node.hpp"

BVHNode::BVHNode(const Box& bounding_box, std::unique_ptr<BVHNode> left_node, std::unique_ptr<BVHNode> right_node) :
                bounding_box_(bounding_box), left_node_(std::move(left_node)), right_node_(std::move(right_node)) {}

BVHNode::BVHNode(const Box& bounding_box, const std::vector<const Object*> objects) :
                bounding_box_(bounding_box), left_node_(nullptr), right_node_(nullptr), objects_(std::move(objects)) {}

bool BVHNode::isLeaf() const {
    if(right_node_ == nullptr && left_node_ == nullptr) {
        return true;
    }
    return false;
}

const Box& BVHNode::GetBoundingBox() const {
    return bounding_box_;
}
