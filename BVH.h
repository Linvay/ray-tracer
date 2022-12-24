#ifndef BVH_H
#define BVH_H

#include <vector>
#include <memory>

class Object;
class BBox;

struct Node;

class BVH {
public:
    BVH(std::vector<std::shared_ptr<BBox>>&);
    ~BVH();

    void build();
    std::vector<std::shared_ptr<Object>> intersect(const vec3 &origin, const vec3 &invDir, const std::vector<bool> &sign, float &tHit);

private:
    Node* head;
    std::vector<std::shared_ptr<BBox>> baseObjects;
};


#endif