#ifndef GRAPH_H
#define GRAPH_H

#include <cstdlib>
#include <set>
#include <list>
#include <vector>
#include <map>
#include <climits>
using namespace std;

#define DOUBLE_MAX numeric_limits<double>::max()

template<typename V, typename E>
class Graph
{
public:
    Graph() {}
    ~Graph() {}
    V* add_vertex(const V &_add);
    /* return true if successfully added*/
    bool add_edge(const E _edge);
    /* erase the related data of every element in _victim */
    void erase_edge(vector<V*> _victims);
    void clear();
    /* return pair of cost and path */
    void Floyd_Warshall();
    pair< double, list<V*> > shortest_path(V *_start, V *_end);
// private: 
    list<V> vertices;
    map<V*, list<E> > edges;
    map< V*, map< V*, pair<double, V*> > > adjMat;
    bool is_up_to_date = true;
};

template<typename V, typename E>
V* Graph<V, E>::add_vertex(const V &_add)
{
    this->is_up_to_date = false;
    this->vertices.push_back(_add);
    V *ptr = &(this->vertices.back());
    this->edges[ptr] = list<E>();
    return ptr;  
}
/* erase edges related to these victims */
template<typename V, typename E>
void Graph<V, E>::erase_edge(vector<V*> _victims)
{
    set<V*> s;
    for(auto victim : _victims){
        s.insert(victim);
    }
    for(auto &edge_start : this->edges){ // edge_start is edge list
        for(auto edge = edge_start.second.begin(); edge != edge_start.second.end();){ // edge
            if(s.find(edge->dst) != s.end() || s.find(edge->src) != s.end()){
                edge = edge_start.second.erase(edge);
            }
            else{
                edge++;
            }
        }
    }
    for(auto victim : _victims){
        this->edges.erase(victim);
    }
    if(!_victims.empty()){
        this->is_up_to_date = false;
    }
}

template<typename V, typename E>
bool Graph<V, E>::add_edge(const E _edge)
{
    if(this->edges.find(_edge.src)!= this->edges.end()){
        this->is_up_to_date = false;
        this->edges[_edge.src].push_back(_edge);
        return true;
    }
    return false;
}

template<typename V, typename E>
void Graph<V, E>::clear()
{
    this->vertices.clear();
    this->is_up_to_date = true;
}

template<typename V, typename E>
void Graph<V, E>::Floyd_Warshall()
{
    // init
    this->adjMat.clear();
    for(auto it : this->edges){
        map< V*, pair<double, V*> > row;
        for(auto it2 : this->edges){
            row[it2.first] = {DOUBLE_MAX, nullptr};
        }
        this->adjMat[it.first] = row;
    }
    for(auto it : this->edges){
        map<V*, int> count;
        for(auto it2 : it.second){
            if(count.find(it2.dst) == count.end()){
                this->adjMat[it.first][it2.dst] = {it2.w, it.first};
                count[it2.dst] = 1;
            }
            else{
                double w_sum = this->adjMat[it.first][it2.dst].first * count[it2.dst];
                count[it2.dst]++;
                this->adjMat[it.first][it2.dst] = { (w_sum + it2.w) / count[it2.dst], it.first};
            }
        }
        this->adjMat[it.first][it.first] = {0, nullptr};
    }
    // body
    for(auto &mid : this->adjMat){
        for(auto &start : this->adjMat){
            for(auto &dst : start.second){
                double new_cost = this->adjMat[start.first][mid.first].first + this->adjMat[mid.first][dst.first].first;
                double ori_cost = this->adjMat[start.first][dst.first].first;
                if(new_cost < ori_cost){
                    this->adjMat[start.first][dst.first].first = new_cost;
                    this->adjMat[start.first][dst.first].second = this->adjMat[mid.first][dst.first].second;
                }
            }
        }
    }
    this->is_up_to_date = true;
}

/*
    start point exclusive
    if both _start and _end are valid vertex pointers
        return pair(cost, path)
    else
        return pair(double_max, empty path)
*/
template<typename V, typename E>
pair<double, list<V*> > Graph<V, E>::shortest_path(V *_start, V *_end)
{
    if(this->edges.find(_start) != this->edges.end() && this->edges.find(_end) != this->edges.end()){
        pair<double, list<V*> > ret;
        if(!(this->is_up_to_date)){
            this->Floyd_Warshall();
        }
        // generate path
        V* ptr = _end;
        ret.second.push_front(_end);
        while(ptr != _start){
            auto p = this->adjMat[_start][ptr];
            ret.first += p.first;
            ret.second.push_front(p.second);
            ptr = p.second;
        }
        // ret.second.push_front(_start);
        return ret;
    }
    else{
        return {DOUBLE_MAX, list<V*>()};
    }
}
#endif