#include "BVH.h"
#include "Object.h"
#include <queue>
#include <iostream>

using namespace std;
struct Node{
    int id = -1;
    BBox* box = nullptr;
    float cost = 0;
    Node *lchild = nullptr, *rchild = nullptr;
};
class cmp {
public:
    int sortAxis;
    cmp(int axis) : sortAxis(axis) {};
    bool operator()(const Node &lhs, const Node &rhs){
        return lhs.cost < rhs.cost;
    }    
};

vector<Node> treeSpace;
vector<BBox> boxSpace;

BVH::BVH(const vector<BBox*> &objs) : baseObjects(objs){

}

float mergeCost(const BBox *_lhs, const BBox *_rhs){
    auto lhs = *_lhs;
    auto rhs = *_rhs;
    BBox merge(lhs + rhs);
    auto box = merge[1] - merge[0];
    float v = box[0] * box[1] * box[2];
    box = rhs[1] - rhs[0];
    v -= box[0] * box[1] * box[2];
    box = lhs[1] - lhs[0];
    v -= box[0] * box[1] * box[2];
    
    box = lhs[1] - rhs[0];
    if(box[0] > 0 && box[1] > 0 && box[2] > 0){
        v += box[0] * box[1] * box[2];
    }
    box = rhs[1] - lhs[0];
    if(box[0] > 0 && box[1] > 0 && box[2] > 0){
        v += box[0] * box[1] * box[2];
    }
    return v;
}

void BVH::build(){
    treeSpace.clear();
    treeSpace.reserve(((int)baseObjects.size()) << 1);
    boxSpace.clear();
    boxSpace.reserve(((int)baseObjects.size()) << 1);
    queue<int> q;
    int treesize = 0;
    if(!q.empty())
        cout<<"ERROR"<<endl;
    for(auto &it : baseObjects){
        if(it == nullptr)
            cout<<"crash!!"<<endl;
        boxSpace.push_back(*it);
        treeSpace.push_back(Node({treesize++, &(boxSpace[boxSpace.size() - 1]), 0, nullptr, nullptr}));
    }
    for(auto &it : treeSpace){
        auto lhs = &it;
        q.push(lhs->id);
    }
    int total = baseObjects.size();
    int next_total = 0;
    while(q.size() > 1) {
        if(total == 1){
            auto n = q.front();
            q.pop();
            q.push(n);
            --total;
            ++next_total;
        }
        if(total == 0){
            total = next_total;
            next_total = 0;
            continue;
        }
        auto &first = treeSpace[q.front()];
        q.pop();
        auto lhs = &first;
        auto &second = treeSpace[q.front()];
        q.pop();
        total -= 2;
        auto rhs = &second;
        boxSpace.push_back(BBox(*(lhs->box) + *(rhs->box)));
        Node tmp = {
            treesize++,
            &(boxSpace[boxSpace.size() - 1]),
            mergeCost(lhs->box, rhs->box) + lhs->cost + rhs->cost,
            lhs,
            rhs
        };
        treeSpace.push_back(tmp);
        q.push(treeSpace.size() - 1);
        ++next_total;
    }
    head = &(treeSpace[treeSpace.size() - 1]);
    return;
}

void BVH::greedybuild(){
    treeSpace.clear();
    treeSpace.reserve(((int)baseObjects.size()) << 1);
    boxSpace.clear();
    boxSpace.reserve(((int)baseObjects.size()) << 1);
    queue<int> q;
    int treesize = 0;
    if(!q.empty())
        cout<<"ERROR"<<endl;
    for(auto &it : baseObjects){
        if(it == nullptr)
            cout<<"crash!!"<<endl;
        boxSpace.push_back(*it);
        treeSpace.push_back(Node({treesize++, &(boxSpace[boxSpace.size() - 1]), 0, nullptr, nullptr}));
    }
    for(auto &it : treeSpace){
        auto lhs = &it;
        q.push(lhs->id);
    }
    int total = baseObjects.size();
    int next_total = 0;
    while(q.size() > 1){
        if(total == 1){
            auto n = q.front();
            q.pop();
            q.push(n);
            --total;
            ++next_total;
        }
        if(total == 0){
            total = next_total;
            next_total = 0;
            continue;
        }
        auto &first = treeSpace[q.front()];
        q.pop();
        auto lhs = &first;
        auto &second = treeSpace[q.front()];
        q.pop();
        total -= 2;
        auto rhs = &second;
        cout<<rhs->cost<<endl;
        boxSpace.push_back(BBox(*(lhs->box) + *(rhs->box)));
        Node tmp = {
            treesize++,
            &(boxSpace[boxSpace.size() - 1]),
            mergeCost(lhs->box, rhs->box) + lhs->cost + rhs->cost,
            lhs,
            rhs
        };
        treeSpace.push_back(tmp);
        q.push(treeSpace.size() - 1);
        ++next_total;
    }
    head = &(treeSpace[treeSpace.size() - 1]);
    return;
}

vector<shared_ptr<Object>> BVH::intersect(
        const std::vector<std::shared_ptr<Object>> &objects,
        const vec3 &origin, const vec3 &dir, const vec3 &invDir,
        const std::vector<bool> &sign, float &tHit){
    vector<shared_ptr<Object>> ret;
    queue<Node*> q;
    q.push(head);
    while(!q.empty()){
        auto now = q.front();
        q.pop();
        if(!now){
            continue;
        }
        if(now->box == nullptr){
            cout<<"null box detected"<<endl;
            continue;
        }

        if(!(now->box->intersect(origin, dir, invDir, sign, tHit))){
            continue;
        }
        if(now->cost < 0.000001){
            ret.insert(ret.end(),
                        objects.begin() + now->box->objl,
                        objects.begin() + now->box->objr);
            continue;
        }

        q.push(now->lchild);
        q.push(now->rchild);
    }
    return ret;
}


