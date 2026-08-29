#include "object.hpp"
#include "box.hpp"
#include <memory>
#include <vector>

class BVHNode {
public:
    BVHNode(const Box& bounding_box, std::unique_ptr<BVHNode> left_node, std::unique_ptr<BVHNode> right_node);
    BVHNode(const Box& bounding_box, const std::vector<const Object*> objects);
    void SetObjects(const Object* objects);
    bool isLeaf() const;
    const Box& GetBoundingBox() const;

private:
    Box bounding_box_;
    std::unique_ptr<BVHNode> left_node_;
    std::unique_ptr<BVHNode> right_node_;
    std::vector<const Object*> objects_;
};
