#ifndef BVH_H
#define BVH_H

#include <vector>
#include <memory>
#include "algebra3.h"

class Object;
class BBox;

struct Node;

class BVH {
public:
    BVH(const std::vector<BBox*>&);
    ~BVH();

    void build();
    std::vector<std::shared_ptr<Object>> intersect(
    const std::vector<std::shared_ptr<Object>> &objects,
        const vec3 &origin, const vec3 &invDir,
        const std::vector<bool> &sign, float &tHit);

private:
    Node* head;
    std::vector<BBox*> baseObjects;
};


#endif