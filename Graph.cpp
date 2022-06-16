#include "Graph.h"

/* Constructor - builds an emtpy graph */
Graph::Graph() {
    this->numberOfStations = 0;
}

/* Copy assignment operator */
Graph& Graph::operator=(const Graph &graph) {
    for (int i=0; i<graph.numberOfStations; i++){
        station st = graph.stationsList[i].first;
        addVertex(st);
    }
    for (int i=0; i<graph.numberOfStations;  i++) {
        station currentStation = graph.stationsList[i].first;
        for (int j = 0; j < 4; j++) {
            for (int n = 0; n < int(graph.stationsList[i].second[j].size()); n++) {
                station st = graph.stationsList[i].second[j][n].first;
                addEdge(currentStation,st,graph.stationsList[i].second[j][n].second,j);
            }
        }
    }
    return *this;
}

/* This function adds station to the graph if he is not exist in the graph */
bool Graph::addVertex(station &st) {
    if (!stationExistById(st)){
        vector<vector<AccessibleStation> > vect(4,vector<AccessibleStation>());
        this->stationsList.push_back(make_pair(st,vect));
        this->numberOfStations++;
        return true;
    }
    return false;
}

/* This function adds path between these two station if he is not exist */
bool Graph::addEdge(station &source, station &destination, int distance, int typeOfVehicle) {
    if (!edgeExist(source,destination,distance,typeOfVehicle)){
        vector<AccessibleStation> &x = this->stationsList[source->getId()].second[typeOfVehicle];
        x.push_back(make_pair(destination,distance));
        return true;
    }
    return false;
}

/* This function checks if path between these two station is exist */
bool Graph::edgeExist(station &source, station &destination, int distance, int typeOfVehicle) {
    vector<AccessibleStation> currentAccessibleStationsVect = stationsList[source->getId()].second[typeOfVehicle];
    for (int i=0; i<int(currentAccessibleStationsVect.size()); i++){
        if (currentAccessibleStationsVect[i].first->getId() == destination->getId()){
            if (currentAccessibleStationsVect[i].second > distance){
                currentAccessibleStationsVect[i].second = distance;
            }
            return true;
        }
    }
    return false;
}

/* This function checks if the station is exist in the graph by their station's id */
bool Graph::stationExistById(station &st) {
    if (st->getId() < numberOfStations)
        return true;
    return false;
}

/* This function checks if the station is exist in the graph by their station's name. Returns the station id or -1 */
int Graph::stationExistByName(string &name) {
    for (int i=0; i<int(stationsList.size()); i++) {
        if (stationsList[i].first->getName().compare(name) == 0)
            return stationsList[i].first->getId();
    }
    return -1;
}

/* Dijkstra implementation to find the shortest path from the source station to any other station */
vector<int> Graph::shortestPath(string &source, typeOfVehicle typeOfVehicle) {
    int src = getStationByName(source)->getId();
    typedef pair<int, int> iPair;
    priority_queue< iPair, vector <iPair> , greater<iPair> > pq;
    vector<int> dist(numberOfStations, INF);
    pq.push(make_pair(0, src));
    dist[src] = 0;

    while (!pq.empty()){
        int u = pq.top().second;
        pq.pop();
        vector<AccessibleStation>::iterator i;
        for (i = stationsList[u].second[typeOfVehicle].begin(); i != stationsList[u].second[typeOfVehicle].end(); ++i){
            int v = i->first->getId();
            int weight = i->second;
            if (dist[v] > dist[u] + weight){
                dist[v] = dist[u] + weight;
                pq.push(make_pair(dist[v], v));
            }
        }
    }
    return dist;
}

/* Dijkstra implementation to find the shortest path from the source station to target station.
 * Returns the distance and the number of stops between the stations that the path contains
 * The path contains only one type of vehicle */
pair<int,int> Graph::shortestPathToSpecificTarget(string &source, int &targetId, typeOfVehicle type) {
    pair<int,int> results(0,0);
    int stationReplacementCounter = 0;
    int sourceForEachStation[numberOfStations];
    int src = getStationByName(source)->getId();
    typedef pair<int, int> iPair;
    priority_queue< iPair, vector <iPair> , greater<iPair> > pq;
    vector<int> dist(numberOfStations, INF);
    pq.push(make_pair(0, src));
    dist[src] = 0;
    while (!pq.empty()){
        int u = pq.top().second;
        pq.pop();
        vector<AccessibleStation>::iterator i;
        for (i = stationsList[u].second[type].begin(); i != stationsList[u].second[type].end(); ++i){
            int v = i->first->getId();
            int weight = i->second;
            if (dist[v] > dist[u] + weight){
                dist[v] = dist[u] + weight;

                sourceForEachStation[v] = stationsList[u].first->getId();
                pq.push(make_pair(dist[v], v));
            }
        }
    }
    int i = targetId;
    if (dist[targetId] != INF){
        while (sourceForEachStation[i] != src){
            stationReplacementCounter++;
            i = sourceForEachStation[i];
        }
        results.first = dist[targetId];
        results.second = stationReplacementCounter;
    }
    return results;
}

/* MultiExpress auxiliary function to creates the mix vehicles graph */
void Graph::multiExpressMixVehicles() {
    pair<station , vector<vector<AccessibleStation> > >* currentStation;
    AccessibleStation as;
    station st;
    int newDistance, temp;
    int elementsCounter = 0;
    for (int i=0; i<this->numberOfStations;  i++){
        currentStation = &this->stationsList[i];
        for (int j=0; j<4; j++){
            temp = elementsCounter;
            for (int n=0; n<int(currentStation->second[j].size())-temp; n++){
                elementsCounter++;
                st = currentStation->second[j][n].first;
                newDistance = currentStation->second[j][n].second;
                as = make_pair(st,newDistance);
                for (int m=0; m<4; m++){
                    if (m!=j){
                        currentStation->second[m].push_back(as);
                    }
                }
            }
        }
        elementsCounter = 0;
    }
}

/* Dijkstra implementation to find the shortest path from the source station to target station.
 * Returns a details vector that contains the path details.
 * The path contains all the types of vehicles */
vector<pair<vector<string>, int> > Graph::shortestPathMultiExpress(string &source, int &targetId, int* stopTimes, int* transitTimes) {
    vector<pair<vector<string>, int> > results;
    station sourceForEachStation[numberOfStations];
    station src = getStationByName(source);
    typedef pair<int, station> iPair;
    priority_queue< iPair, vector <iPair> , greater<iPair> > pq;
    vector<int> dist(numberOfStations, INF);
    pq.push(make_pair(0, src));
    dist[src->getId()] = 0;
    typeOfVehicle currentTypeOfVehicle, nextTypeOfVehicle;
    while (!pq.empty()){
        station currentStation = pq.top().second;
        int u = currentStation->getId();
        currentTypeOfVehicle = currentStation->getTypeOfVehicle();
        pq.pop();
        vector<AccessibleStation>::iterator i;
        for (i = stationsList[u].second[0].begin(); i != stationsList[u].second[0].end(); ++i){
            station &v = i->first;
            int id = v->getId();
            nextTypeOfVehicle = v->getTypeOfVehicle();
            int weight = i->second;
            if (currentTypeOfVehicle != general && currentTypeOfVehicle != nextTypeOfVehicle)
                weight += transitTimes[v->getTypeOfStation()];
            else if (currentTypeOfVehicle == nextTypeOfVehicle)
                weight += stopTimes[nextTypeOfVehicle];
            if (dist[id] > dist[u] + weight){
                dist[id] = dist[u] + weight;
                if (currentStation->getTypeOfVehicle() != v->getTypeOfVehicle()){
                    station st(new Station(*currentStation.get()));
                    currentStation = st;
                    currentStation->setTypeOfVehicle(v->getTypeOfVehicle());
                }
                sourceForEachStation[id] = currentStation;
                pq.push(make_pair(dist[id], v));
            }
        }
    }
    int i = targetId;
    if (dist[targetId] != INF){
        string st = stationsList[targetId].first->getName();
        station previousStation = sourceForEachStation[i];
        typeOfVehicle previousStationType = previousStation->getTypeOfVehicle();
        vector<string> details;
        while (previousStation != NULL){
            details.push_back(stationsList[i].first->getName());
            if (previousStationType == bus)
                details.push_back("bus");
            else if (previousStationType == tram)
                details.push_back("tram");
            else if (previousStationType == sprinter)
                details.push_back("sprinter");
            else if (previousStationType == rail)
                details.push_back("rail");
            typeOfStation tos = stationsList[i].first->getTypeOfStation();
            if (tos == stad)
                details.push_back("stad");
            else if (tos == centraal)
                details.push_back("centraal");
            else if (tos == intercity)
                details.push_back("intercity");
            if (i != targetId && (previousStationType == sourceForEachStation[i]->getTypeOfVehicle() || sourceForEachStation[i]->getTypeOfVehicle() == general))
                results.push_back(make_pair(details,dist[i]-dist[sourceForEachStation[i]->getId()]));
            else
                results.push_back(make_pair(details,dist[i]-dist[sourceForEachStation[i]->getId()]));
            i = sourceForEachStation[i]->getId();
            if (sourceForEachStation[i] != NULL){
                previousStation = sourceForEachStation[i];
                previousStationType = sourceForEachStation[i]->getTypeOfVehicle();
            }
            else previousStation = NULL;
            details.clear();
        }
        details.push_back(src->getName());
        results.push_back(make_pair(details,dist[i]));
    }
    return results;
}

/* This functions prints the graph details to an external File */
void Graph::printGraph(string &file) const {
    ofstream outputFile;
    outputFile.open(file);
    for (int i=0; i<numberOfStations; i++){
        outputFile << "from: " << stationsList[i].first->getName() << endl;
        for (int n=0; n<4; n++){
            if (stationsList[i].second[n].size() != 0){
                if (n == 0){
                    outputFile << "bus:";
                }
                else if (n == 1){
                    outputFile << "tram:";
                }
                else if (n == 2){
                    outputFile << "sprinter:";
                }
                else if (n == 3){
                    outputFile << "rail:";
                }
                for (int j=0; j<int(stationsList[i].second[n].size()); j++){
                    outputFile << " " << stationsList[i].second[n][j].first->getName();
                }
                outputFile << endl;
            }
        }
        outputFile << "_______________________________________________________\n" << endl;
    }
}

/* This function finds station by her name and return it */
station Graph::getStationByName(string &name){
    for (int i=0; i<int(stationsList.size()); i++) {
        if (stationsList[i].first->getName().compare(name) == 0)
            return stationsList[i].first;
    }
    station st = nullptr;
    return st;
}

/* This function returns the station list of the graph */
vector<station> Graph::getStationsList() {
    vector<station> stationList;
    for (int i=0; i<numberOfStations; i++){
        stationList.push_back(stationsList[i].first);
    }
    return stationList;
}