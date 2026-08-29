#include "bvh.hpp"
#include "object.hpp"
#include "box.hpp"
#include "triangle.hpp"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <optional>

const size_t kBucketsCount = 16;
const size_t kLeafThreshold = 4;

namespace {

struct Bucket {
    Bucket() : bounding_box(Box::Empty()), count(0) {}
    Box bounding_box;
    size_t count;
};

struct SplitCandidate {
    double cost;
    size_t bucket_idx;
};

std::array<Bucket, kBucketsCount> DivideIntoBuckets(const std::vector<const Object*>& objects, const Box& parent_box, int axis);
std::optional<SplitCandidate> FindBestSplit(const std::array<Bucket, kBucketsCount>& buckets);
size_t GetBucketNum(double centroid_coord, double min, double max);

} // namespace

BVHNode BuildBVH(const std::vector<const Object*>& objects, const Box& parent_box) {
    if(objects.size() <= kLeafThreshold) {
        return BVHNode(parent_box, objects);
    }
    Box centroid_bound = Box::Empty();
    for(const auto object : objects) {
        centroid_bound.Extend(object->polygon.Centroid());
    }
    double best_cost = std::numeric_limits<double>::infinity();
    size_t best_idx = 0;
    int best_axis = 0;
    for(int axis = 0; axis < 3; axis++) {
        auto buckets = DivideIntoBuckets(objects, centroid_bound, axis);
        auto split = FindBestSplit(buckets);
        if(split.has_value() && split->cost < best_cost) {
            best_cost = split->cost;
            best_idx = split->bucket_idx;
            best_axis = axis;
        }
    }
    if(best_cost == std::numeric_limits<double>::infinity()) {
        return BVHNode(parent_box, objects);
    }
    auto mid = std::partition(objects.begin(), objects.end(), [&](const Object* obj) {
        return GetBucketNum(obj->polygon.Centroid()[best_axis], min, max) <= best_idx;
    })
}

namespace {

std::array<Bucket, kBucketsCount> DivideIntoBuckets(const std::vector<const Object*>& objects, const Box& bound, int axis) {
    std::array<Bucket, kBucketsCount> buckets;
    double min = bound.GetMin()[axis];
    double max = bound.GetMax()[axis];
    if(max <= min) {
        return buckets;
    }
    for(const auto object : objects) {
        double fraction = (object->polygon.Centroid()[axis] - min) / (max - min);
        fraction = std::max(0.0, fraction);
        size_t bucket_num = static_cast<size_t>(std::floor(kBucketsCount * fraction));
        bucket_num = std::min(bucket_num, kBucketsCount - 1);
        buckets[bucket_num].bounding_box.Extend(object->polygon);
        buckets[bucket_num].count++;
    }
    return buckets;
}

std::optional<SplitCandidate> FindBestSplit(const std::array<Bucket, kBucketsCount>& buckets) {
    double best_cost = std::numeric_limits<double>::infinity();
    size_t best_idx = 0;
    std::array<double, kBucketsCount> left_box_sa;
    Box left_acc = Box::Empty();
    size_t triangle_count = 0;
    for(size_t i = 0; i < kBucketsCount; i++) {
        left_acc.Extend(buckets[i].bounding_box);
        left_box_sa[i] = left_acc.SurfaceArea();
        triangle_count += buckets[i].count;
    }
    Box right_acc = Box::Empty();
    size_t right_count = 0;
    for(size_t i = kBucketsCount - 1; i >= 1; i--) {
        right_acc.Extend(buckets[i].bounding_box);
        right_count += buckets[i].count;
        size_t left_count = triangle_count - right_count;
        if(left_count == 0 || right_count == 0) {
            continue;
        }
        double cost = left_box_sa[i-1] * left_count + right_acc.SurfaceArea() * right_count;
        if(best_cost > cost) {
            best_cost = cost;
            best_idx = i-1;
        }
    }
    if(best_cost == std::numeric_limits<double>::infinity()) {
        return std::nullopt;
    }
    return SplitCandidate{best_cost, best_idx};
}

size_t GetBucketNum(double centroid_coord, double min, double max) {
    double fraction = (centroid_coord - min) / (max - min);
    fraction = std::max(0.0, fraction);
    size_t bucket_num = static_cast<size_t>(std::floor(kBucketsCount * fraction));
    return std::min(bucket_num, kBucketsCount - 1);
}

} // namespace