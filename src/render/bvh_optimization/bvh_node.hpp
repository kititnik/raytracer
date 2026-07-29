#include "object.hpp"
#include "rectangle.hpp"
#include <memory>
#include <vector>

class BVHNode {
public:
    bool isLeaf() const;
    const Rectangle& GetBoundingBox() const;

private:
    Rectangle bounding_box_;
    std::unique_ptr<BVHNode> left_node_;
    std::unique_ptr<BVHNode> right_node_;
    std::vector<const Object*> objects_;
};
