#include <iostream>
#include <sstream>
#include "graphiso.h"

using namespace std;

// ================= Complete - Begin Graph class implementation ===================
Graph::Graph(std::istream& istr)
{
    string aline;
    while(getline(istr,aline))
    {
        istringstream iss(aline);
        string u, v;
        if(iss >> u){
            VERTEX_SET_T neighbors;
            while(iss >> v)
            {
                neighbors.insert(v);
            }
            adj_.insert(std::make_pair(u,neighbors));
        }
    }
}


bool Graph::edgeExists(const VERTEX_T& u, const VERTEX_T& v) const
{
    if((adj_.find(u) != adj_.end()) && (adj_.at(u).find(v) != adj_.at(u).end()))
    {
        return true;
    }
    return false;
}
const VERTEX_SET_T& Graph::neighbors(const VERTEX_T& v) const
{
    if(adj_.find(v) == adj_.end()){
        throw std::invalid_argument("Neighbors: invalid vertex - not in  map");
    }
    return adj_.at(v);
}
VERTEX_LIST_T Graph::vertices() const
{
    VERTEX_LIST_T verts;
    for(const auto& p : adj_)
    {
        verts.push_back(p.first);
    }
    return verts;
}
// ================= Complete - End Graph class implementation ===================



// To be completed
bool isConsistent(const Graph& g1, const Graph& g2, VERTEX_ID_MAP_T& mapping)
{
    // Feel free to change or remove this starter code
    VERTEX_LIST_T g1verts = g1.vertices();

    // cout << "-- isConsistent" << endl;
    // for(const auto& g1v : g1verts) cout << "* " << g1v << ", " << mapping[g1v]  << endl;

    for(const auto& g1u : g1verts)
    {
        // Check mappings for necessary vertices to see if there is any violation
        // and return false
        VERTEX_T g2u = mapping[g1u];
        VERTEX_SET_T g1neighbors = g1.neighbors(g1u);
        VERTEX_SET_T g2neighbors = g2.neighbors(g2u);
        if (g1neighbors.size() != g2neighbors.size()) return false;
        for (VERTEX_SET_T::iterator it=g1neighbors.begin(); it != g1neighbors.end(); it++){
            VERTEX_SET_T::iterator jt = g2neighbors.find(mapping[*it]);
            if (jt == g2neighbors.end()) return false;
        }
    }
    return true;
}

// Add any helper functions you deem useful

bool graphIsoRec(size_t idx, const Graph& g1, const Graph& g2, VERTEX_ID_MAP_T& mapping)
{
   if (idx >= g2.vertices().size()) {
        VERTEX_LIST_T g1verts = g1.vertices();

        // for(const auto& g1v : g1verts) cout << "- " << g1v << ", " << mapping[g1v]  << endl;
        return (isConsistent(g1, g2, mapping));
    }
    
    if (idx > 0) {
        VERTEX_T prevV1 = g1.vertices()[idx-1];
        VERTEX_SET_T v1neighbors = g1.neighbors(prevV1);
        VERTEX_SET_T v2neighbors = g2.neighbors(mapping[prevV1]);
        if (v1neighbors.size() != v2neighbors.size()) {
            // cout << "-false" << endl;
            return false;
        } 
    }

    for (size_t i = idx; i < g2.vertices().size(); ++i) {
        // swap idx position and i (iteration)
        VERTEX_T v1Idx = g1.vertices()[idx];
        VERTEX_T v1i = g1.vertices()[i];
        VERTEX_T tmp = mapping[v1Idx];
        mapping.insert({v1Idx, mapping[v1i]});
        mapping.insert({v1i, tmp});

        if(graphIsoRec(idx + 1, g1, g2, mapping)) return true;

        // revert the change after recursion swap idx and i
        v1Idx = g1.vertices()[idx];
        v1i = g1.vertices()[i];
        tmp = mapping[v1Idx];
        mapping.insert({v1Idx, mapping[v1i]});
        mapping.insert({v1i, tmp});
    }
    return false;
}

// // To be completed
bool graphIso(const Graph& g1, const Graph& g2, VERTEX_ID_MAP_T& mapping)
{
    VERTEX_LIST_T g1verts = g1.vertices();
    if(g1verts.size() != g2.vertices().size())
    {
        return false;
    }
    // Add code here
    for (size_t i = 0; i < g1.vertices().size(); i++) {
        mapping.insert({g1.vertices()[i], g2.vertices()[i]});
    }
    return graphIsoRec(0, g1, g2, mapping);

    // Delete this and return the correct value...
    // This is just placeholder to allow compilation
    // return false;
}


