#include "BVH.h"
#include "Object.h"
#include <queue>

using namespace std;

struct Node{
    shared_ptr<BBox> box;
    float cost;
    Node *lchild, *rchild;
};

vector<Node> treeSpace;

BVH::BVH(vector<shared_ptr<BBox>> &objs) : baseObjects(objs){

}

float mergeCost(BBox &lhs, BBox &rhs){
    BBox merge = lhs + rhs;
    auto box = merge[1] - merge[0];

    float v = box[0] * box[1] * box[2];
    box = rhs[1] - rhs[0];
    v -= box[0] * box[1] * box[2];
    box = lhs[1] - lhs[0];
    v -= box[0] * box[1] * box[2];
    if(box.length() > (box.normalize() * (rhs[0] - lhs[0]))){
        box = rhs[0] - lhs[1];
        v += box[0] * box[1] * box[2];
    }
    return v;
}

void BVH::build(){
    treeSpace.clear();
    for(auto &it : baseObjects){
        treeSpace.push_back({it, 0, nullptr, nullptr});
    }
    int total = baseObjects.size();
    for(int i = 0; i < treeSpace.size(); i += 2){
        auto lhs = treeSpace[i].box;
        auto rhs = treeSpace[i + 1].box;
        treeSpace.push_back({make_shared<BBox>(*lhs + *rhs), mergeCost(*lhs, *rhs), &(treeSpace[i]), &(treeSpace[i + 1])});
    }
    head = &(treeSpace[treeSpace.size() - 1]);
    return;
}

vector<shared_ptr<Object>> BVH::intersect(const vec3 &origin, const vec3 &invDir, const std::vector<bool> &sign, float &tHit){
    vector<shared_ptr<Object>> ret;
    queue<Node*> q;
    q.push(head);
    while(!q.empty()){
        auto now = q.front();
        q.pop();
        if(!now->box->intersect(origin, invDir, sign, tHit)){
            continue;
        }
        if(now->cost < 0.01){
            auto list = now->box->Objects();
            ret.insert(ret.end(), list.begin(), list.end());
            continue;
        }
        q.push(now->lchild);
        q.push(now->rchild);
    }
    return ret;
}


