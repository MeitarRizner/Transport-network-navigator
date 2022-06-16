#ifndef EX2_PUBLICTRANSPORTNETWORK_H
#define EX2_PUBLICTRANSPORTNETWORK_H

#include "Graph.h"
#include <fstream>
#include <iterator>
#include <string>
#include <sstream>

/* This class represent a transport network */
/* I used the defaults 'the big three' except constructor */
class PublicTransportNetwork {
private:
    /* members */
    Graph transportNetwork;
    string outputFile;
    int stopTimes[4];
    int transitTimes[3];

    /* internal functions */
    void startProgram(int argc,char** argv);
    int checkCommand(vector<string> vect);
    void getFile(string &fileName);
    void updateGraph(string &source, string &destination, int distance, typeOfVehicle currentTypeOfVehicle);
    station createStation(string &name, typeOfVehicle vehicleType);
    void setConfigFile(string &fileName);
    void setOutputFile(string &fileName);
    void printAccessibleStationsOutbound(vector<station> &stationsList, vector<int> &adjShortestPath, string &type);
    void printAccessibleStationsInbound(vector<string> &adjVehicle , string type);
    void printPath(vector<pair<vector<string>, int> > results);
    vector<string> parseString(const string& s, const string& del);

    /* exceptions */
    class invalidCommandLine{};
    class fileNotFound{};
    class invalidFileName{};
    class invalidInput{};
    class invalidConfigFile{};
    class invalidCommand{};

public:
    PublicTransportNetwork(int argc,char** argv);
    void load(string &fileName);
    void outbound(string &stationName);
    void inbound(string &stationName);
    void uniExpress(string &sourceStation, string &targetStation);
    void multiExpress(string &sourceStation, string &targetStation);
    void print();
};

#endif //EX2_PUBLICTRANSPORTNETWORK_H