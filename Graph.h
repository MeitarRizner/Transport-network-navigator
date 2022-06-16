#ifndef EX2_GRAPH_H
#define EX2_GRAPH_H

#include <iostream>
#include <vector>
#include <list>
#include <memory>
#include <queue>
#include <set>
#include <fstream>
#include "Station.h"
using namespace std;

typedef shared_ptr<Station> station;
typedef pair<station,int> AccessibleStation;
#define INF 0x3f3f3f3f

/* This class represent a graph */
/* I used the defaults 'the big three' except constructor and copy assignment operator */
class Graph {
private:
    /* members */
    int numberOfStations;
    vector<pair<station , vector<vector<AccessibleStation> > > > stationsList;

    /* internal functions */
    bool stationExistById(station &st);
    bool edgeExist(station &source, station &destination, int distance, int typeOfVehicle);

public:
    Graph();
    Graph& operator=(const Graph &graph);
    bool addVertex(station &st);
    bool addEdge(station &source, station &destination, int distance, int typeOfVehicle);

    int stationExistByName(string &name);
    station getStationByName(string &name);
    vector<station> getStationsList();
    vector<int> shortestPath(string &source, typeOfVehicle typeOfVehicle);
    pair<int,int> shortestPathToSpecificTarget(string &source, int &targetId, typeOfVehicle type);
    vector<pair<vector<string>, int> > shortestPathMultiExpress(string &source, int &targetId, int* stopTimes, int* transitTimes);
    void multiExpressMixVehicles();
    void printGraph(string &file) const;
};

#endif //EX2_GRAPH_H