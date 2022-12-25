#include "BVH.h"
#include "Object.h"
#include <queue>
#include <iostream>

using namespace std;

struct Node{
    shared_ptr<BBox> box;
    float cost;
    Node *lchild, *rchild;
};

vector<Node> treeSpace;

BVH::BVH(const vector<shared_ptr<BBox>> &objs) : baseObjects(objs){

}

void destruct(Node *head){
    if(head->lchild)
        destruct(head->lchild);
    if(head->rchild)
        destruct(head->rchild);
    delete head;
}

BVH::~BVH(){
    destruct(head);
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
    queue<Node*> q;
    for(auto &it : baseObjects){
        q.push(new Node({it, 0, nullptr, nullptr}));
    }
    int total = baseObjects.size();
    int next_total = 0;
    int treesize = q.size();
    while(q.size() > 1){
        ++treesize;
        // cout<<q.size()<<" "<<total<<" "<<next_total<<endl;
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
        auto lhs = q.front();
        q.pop();
        auto rhs = q.front();
        q.pop();
        total -= 2;
        q.push(new Node({
            make_shared<BBox>(*(lhs->box) + *(rhs->box)),
            mergeCost(*(lhs->box), *(rhs->box)),
            lhs,
            rhs
        }));
        ++next_total;
    }
    cout<<"Treesize: "<<treesize<<endl;
    head = &(treeSpace[treeSpace.size() - 1]);
    return;
}

vector<shared_ptr<Object>> BVH::intersect(const vec3 &origin, const vec3 &invDir, const std::vector<bool> &sign, float &tHit){
    vector<shared_ptr<Object>> ret;
    queue<Node*> q;
    q.push(head);
    while(!q.empty()){
        cout<<q.size()<<endl;
        auto now = q.front();
        q.pop();
        cout<<"checkhit"<<endl;
        // if(!now->box)
        //     cout<<"nullptr!!"<<endl;
        // else
        cout<<&(now)<<endl;
        cout<<&(now->box)<<endl;
        cout<<(void *)(nullptr)<<endl;
        cout<<now->cost<<endl;
        // auto box = now->box;
        if(!(now->box->intersect(origin, invDir, sign, tHit))){
            cout<<"!"<<endl;
            continue;
        }
        cout<<"hit"<<endl;
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


