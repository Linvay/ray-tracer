#include "BVH.h"
#include "Object.h"
#include <queue>
#include <iostream>

using namespace std;

struct Node{
    BBox* box;
    float cost;
    Node *lchild, *rchild;
};

vector<Node> treeSpace;
vector<BBox> boxSpace;

BVH::BVH(const vector<BBox*> &objs) : baseObjects(objs){

}

// void destruct(Node *head){
//     if(head->lchild)
//         destruct(head->lchild);
//     if(head->rchild)
//         destruct(head->rchild);
//     delete head;
// }

// BVH::~BVH(){
//     if(head->lchild)
//         destruct(head->lchild);
//     if(head->rchild)
//         destruct(head->rchild);
// }

float mergeCost(const BBox *_lhs, const BBox *_rhs){
    // cout<<"!#$"<<endl;
    // lhs + rhs;
    // cout<<"---"<<endl;
    // cout<<_lhs<<" "<<_rhs<<endl;
    auto lhs = *_lhs;
    auto rhs = *_rhs;
    BBox merge(lhs + rhs);
    // cout<<"!#$0"<<endl;
    auto box = merge[1] - merge[0];

    // cout<<"!#$1"<<endl;
    float v = box[0] * box[1] * box[2];
    // cout<<"!#$2"<<endl;
    box = rhs[1] - rhs[0];
    // cout<<"!#$3"<<endl;
    v -= box[0] * box[1] * box[2];
    // cout<<"!#$4"<<endl;
    box = lhs[1] - lhs[0];
    // cout<<"!#$5"<<endl;
    v -= box[0] * box[1] * box[2];
    // cout<<"!#$6"<<endl;
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
        if(!it)
            cout<<"crash!!"<<endl;
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
        // cout<<lhs<<endl;
        // cout<<lhs->box<<endl;
        // cout<<lhs->cost<<endl;
        auto rhs = q.front();
        q.pop();
        total -= 2;
        // cout<<rhs<<endl;
        // cout<<rhs->box<<endl;
        // cout<<rhs->cost<<endl;
        boxSpace.push_back(*(lhs->box) + *(rhs->box));
        Node tmp = {
            // make_shared<BBox>(*(lhs->box) + *(rhs->box)),
            &(boxSpace[boxSpace.size() - 1]),
            // lhs->box,
            // mergeCost(*(lhs->box), *(rhs->box)) + lhs->cost + rhs->cost,
            mergeCost(lhs->box, rhs->box) + lhs->cost + rhs->cost,
            // lhs->cost + rhs->cost,
            lhs,
            rhs
        };
        // cout<<"!"<<endl;
        treeSpace.push_back(tmp);
        // cout<<treeSpace.size()<<endl;
        q.push(&(treeSpace[treeSpace.size() - 1]));
        ++next_total;
    }
    cout<<"Treesize: "<<treesize<<endl;
    head = &(treeSpace[treeSpace.size() - 1]);
    return;
}

vector<shared_ptr<Object>> BVH::intersect(
        const std::vector<std::shared_ptr<Object>> &objects,
        const vec3 &origin, const vec3 &invDir,
        const std::vector<bool> &sign, float &tHit){
    vector<shared_ptr<Object>> ret;
    queue<Node*> q;
    q.push(head);
    // cout<<"cost"<<head->cost<<endl;
    while(!q.empty()){
        // cout<<q.size()<<endl;
        auto now = q.front();
        q.pop();
        if(!now){
            continue;
        }
        // cout<<"checkhit"<<endl;
        // if(!now->box)
        //     cout<<"nullptr!!"<<endl;
        // else
        // cout<<&(now)<<endl;
        // cout<<&(now->box)<<endl;
        // cout<<(void *)(nullptr)<<endl;
        // cout<<now->cost<<endl;
        // auto box = now->box;
        if(!(now->box->intersect(origin, invDir, sign, tHit))){
            // cout<<"!"<<endl;
            continue;
        }
        // cout<<"hit"<<endl;
        // if(now->cost < 0.01){
        //     ret.insert(ret.end(),
        //                 objects.begin() + now->box->objl,
        //                 objects.begin() + now->box->objr);
        //     continue;
        // }
        q.push(now->lchild);
        q.push(now->rchild);
    }
    return ret;
}


