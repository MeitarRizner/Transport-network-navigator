#include "PublicTransportNetwork.h"

/* Constructor - calls 'startProgram' to check if the command line's input is legal, then waits for input and checks it by 'checkCommand' */
PublicTransportNetwork::PublicTransportNetwork(int argc,char** argv) : outputFile("output.dat") {
    stopTimes[bus] = 1;
    stopTimes[tram] = 2;
    stopTimes[sprinter] = 3;
    stopTimes[rail] = 5;
    transitTimes[intercity] = 15;
    transitTimes[centraal] = 10;
    transitTimes[stad] = 5;
    try{
        startProgram(argc,argv);
    }
    catch(fileNotFound exc){
        cerr << "ERROR opening one of the file.\n";
        exit(1);
    }
    catch(invalidFileName exc){
        cerr << "ERROR file name is invalid.\n";
        exit(1);
    }
    catch(invalidInput exc){
        cerr << "ERROR file contents are invalid.\n";
        exit(1);
    }
    catch (invalidCommandLine exc){
        cerr << "ERROR compilation line is invalid.\n";
        exit(1);
    }
    catch (invalidConfigFile exc){
        cerr << "ERROR config file is invalid.\n";
        exit(1);
    }
    vector<string> Query;
    string command = "";
    int cmd;
    while (true) {
        getline(cin, command);
        Query = parseString(command, " ");
        try {
            cmd = checkCommand(Query);
            if (cmd == 1)
                load(Query[1]);
            else if (cmd == 2)
                outbound(Query[1]);
            else if (cmd == 3)
                inbound(Query[1]);
            else if (cmd == 4)
                uniExpress(Query[1], Query[2]);
            else if (cmd == 5)
                multiExpress(Query[1], Query[2]);
            else if (cmd == 6)
                print();
            else if (cmd == 7)
                break;
        }
        catch (invalidCommand ex){
            cerr << "invalid command, try again\n";
        }
    }
    exit(1);
}

/* Checks if tha data from the command line is legal, throws exception if not  */
void PublicTransportNetwork::startProgram(int argc, char **argv) {
    if (argc < 3){
        throw invalidCommandLine();
    }
    int i = 1;
    string args(argv[i]);
    if (args.compare("-i") != 0)
        throw invalidCommandLine();
    args = argv[++i];
    if (args.compare("-c") == 0)
        throw invalidCommandLine();
    while (args.compare("-c") != 0 && args.compare("-o") != 0 && i < argc) {
        getFile(args);
        if (i+1 < argc)
            args = argv[++i];
        else break;
    }
    if (args.compare("-c") == 0){
        if (i+1 < argc){
            args = argv[++i];
            setConfigFile(args);
        }
        else
            throw invalidCommandLine();
        i++;
    }
    if (i+1 < argc){
        args = argv[i];
        if (args.compare("-o") == 0){
            i++;
            if (i < argc){
                args = argv[i];
                setOutputFile(args);
            }
            else
                throw invalidCommandLine();
        }
        else
            throw invalidCommandLine();
    }
}

/* This function checks if the command line's input is legal */
int PublicTransportNetwork::checkCommand(vector<string> vect) {
    if (vect.size() == 1){
        if (vect[0] == "")
            throw invalidCommand();
        if (vect[0].compare("print") == 0)
            return 6;
        if (vect[0].compare("EXIT") == 0)
            return 7;
    }
    else if (vect.size() == 2){
        if (vect[0].compare("load") == 0){
            return 1;
        }
        else if (vect[0].compare("outbound") == 0){
            if (transportNetwork.stationExistByName(vect[1]) == -1){
                cerr << vect[1] << " does not exist in the current network.\n";
                return 0;
            }
            else return 2;
        }
        else if (vect[0].compare("inbound") == 0){
            if (transportNetwork.stationExistByName(vect[1]) == -1){
                cerr << vect[1] << " does not exist in the current network.\n";
                return 0;
            }
            else return 3;
        }
    }
    else if (vect.size() == 3){
        if (vect[1].compare(vect[2]) == 0)
            throw invalidCommand();
        if (vect[0].compare("uniExpress") == 0){
            if (transportNetwork.stationExistByName(vect[1]) == -1){
                cerr << vect[1] << " does not exist in the current network.\n";
                return 0;
            }
            else if (transportNetwork.stationExistByName(vect[2]) == -1){
                cerr << vect[2] << " does not exist in the current network.\n";
                return 0;
            }
            else return 4;
        }
        else if (vect[0].compare("multiExpress") == 0){
            if (transportNetwork.stationExistByName(vect[1]) == -1){
                cerr << vect[1] << " does not exist in the current network.\n";
                return 0;
            }
            else if (transportNetwork.stationExistByName(vect[2]) == -1){
                cerr << vect[2] << " does not exist in the current network.\n";
                return 0;
            }
            else return 5;
        }
    }
    throw invalidCommand();
}

/* This function gets a file name and inserts all the file data into the graph by using 'updateGraph' func  */
void PublicTransportNetwork::getFile(string &fileName) {
    typeOfVehicle currentTypeOfVehicle;
    if(fileName.find("bus") == 0){
        currentTypeOfVehicle = bus;
    }
    else if(fileName.find("tram") == 0){
        currentTypeOfVehicle = tram;
    }
    else if(fileName.find("sprinter") == 0){
        currentTypeOfVehicle = sprinter;
    }
    else if(fileName.find("rail") == 0){
        currentTypeOfVehicle = rail;
    }
    else{
        throw invalidFileName();
    }
    ifstream input(fileName);
    if (!input){
        throw fileNotFound();
    }
    string data;
    vector<string> content;
    while (getline(input,data)) {
        content = parseString(data,"\t");
        if (content.size() != 3) {
            throw invalidInput();
        }
        string source = content[0], destination = content[1], temporaryDistance = content[2];
        for (int i = 0; i < int(source.size()); i++) {
            if (!isalpha(source[i]) && source[i] != '-')
                throw invalidInput();
        }
        for (int i = 0; i < int(destination.size()); i++) {
            if (!isalpha(destination[i]) && destination[i] != '-')
                throw invalidInput();
        }
        for (int i = 0; i < int(temporaryDistance.size()); i++) {
            if (!isdigit(temporaryDistance[i]))
                throw invalidInput();
        }
        int distance = stoi(temporaryDistance);
        if (distance < 0)
            throw invalidInput();
        if (source.size() > 32 || destination.size() > 32)
            throw invalidInput();
        updateGraph(source,destination,distance,currentTypeOfVehicle);
        content.clear();
    }
}

/* This function gets a two station names and inserts them and a road between them into the graph. The type of road is provide.  */
void PublicTransportNetwork::updateGraph(string &source, string &destination, int distance, typeOfVehicle currentTypeOfVehicle) {
    station src;
    station dst;
    if (transportNetwork.stationExistByName(source) == -1){
        src = createStation(source, general);
        transportNetwork.addVertex(src);
    }
    else
        src = transportNetwork.getStationByName(source);
    if (transportNetwork.stationExistByName(destination)== -1){
        dst = createStation(destination, general);
        transportNetwork.addVertex(dst);
    }
    else
        dst = transportNetwork.getStationByName(destination);
    station st(new Station(*dst.get()));
    st->setTypeOfVehicle(currentTypeOfVehicle);
    transportNetwork.addEdge(src,st,distance,currentTypeOfVehicle);
}

/* This function creates a new station */
station PublicTransportNetwork::createStation(string &name, typeOfVehicle vehicleType) {
    typeOfStation stationType;
    if (name.find("IC") == 0){
        stationType = intercity;
    }
    else if(name.find("CS") == 0){
        stationType = centraal;
    }
    else {
        stationType = stad;
    }
    shared_ptr<Station> currentStation(new Station(name, stationType, vehicleType));
    return currentStation;
}

/* This function is used only if the user provided a config file */
void PublicTransportNetwork::setConfigFile(string &fileName) {
    ifstream input(fileName);
    if (!input){
        throw fileNotFound();
    }
    string data;
    vector<string> content;
    while (getline(input,data)) {
        content = parseString(data," ");
        if (content.size() != 2) {
            throw invalidConfigFile();
        }
        for (int i = 0; i < int(content[0].size()); i++) {
            if (!isalpha(content[0][i]))
                throw invalidConfigFile();
        }
        for (int i = 0; i < int(content[1].size()); i++) {
            if (!isdigit(content[1][i]))
                throw invalidConfigFile();
        }
        if (content[0].compare("bus") == 0){
            stopTimes[bus] = stoi(content[1]);
        }
        else if (content[0].compare("tram") == 0){
            stopTimes[tram] = stoi(content[1]);
        }
        else if (content[0].compare("sprinter") == 0){
            stopTimes[sprinter] = stoi(content[1]);
        }
        else if (content[0].compare("rail") == 0){
            stopTimes[rail] = stoi(content[1]);
        }
        else if (content[0].compare("intercity") == 0){
            transitTimes[intercity] = stoi(content[1]);;
        }
        else if (content[0].compare("centraal") == 0){
            transitTimes[centraal] = stoi(content[1]);;
        }
        else if (content[0].compare("stad") == 0){
            transitTimes[stad] = stoi(content[1]);;
        }
        else
            throw invalidConfigFile();
        content.clear();
    }
}

/* This function is used only if the user provided an alternative output file */
void PublicTransportNetwork::setOutputFile(string &fileName) {
    this->outputFile = fileName;
}

/* Query number one - load a data file */
void PublicTransportNetwork::load(string &fileName) {
    try{
        getFile(fileName);
        cout << "Update was successful.\n";
    }
    catch(fileNotFound exc){
        cerr << "ERROR opening the specified file.\n";
    }
    catch(invalidFileName exc){
        cerr << "ERROR file name is invalid.\n";
    }
    catch(invalidInput exc){
        cerr << "ERROR file contents are invalid.\n";
    }
}

/* Query number two - the group of stations that can be reached from a specific station*/
void PublicTransportNetwork::outbound(string &stationName) {
    vector<int> adjBus, adjTram, adjSprinter, adjRail;
    vector<station> stationsList;
    adjBus = transportNetwork.shortestPath(stationName,bus);
    adjTram = transportNetwork.shortestPath(stationName,tram);
    adjSprinter = transportNetwork.shortestPath(stationName,sprinter);
    adjRail = transportNetwork.shortestPath(stationName,rail);
    stationsList = transportNetwork.getStationsList();
    string b = "bus", t = "tram", s = "sprinter", r = "rail";
    printAccessibleStationsOutbound(stationsList,adjBus,b);
    printAccessibleStationsOutbound(stationsList,adjTram,t);
    printAccessibleStationsOutbound(stationsList,adjSprinter,s);
    printAccessibleStationsOutbound(stationsList,adjRail,r);
}

/* This function prints the accessible stations from a specific station (outbound auxiliary function) */
void PublicTransportNetwork::printAccessibleStationsOutbound(vector<station> &stationsList, vector<int> &adjShortestPath,
                                                             string &type) {
    bool flag = false;
    int i = 0;
    cout << type <<  ": ";
    while(i < int(stationsList.size())){
        if (adjShortestPath[i] != INF && adjShortestPath[i] != 0){
            cout << stationsList[i]->getName() << '\t';
            flag = true;
        }
        i++;
    }
    if (!flag)
        cout << "no outbound travel\n";
    else
        cout << "\n";
}

/* Query number three - The group of stations that there is a way from them to a specific station */
void PublicTransportNetwork::inbound(string &stationName) {
    vector<station> stationsList = transportNetwork.getStationsList();
    vector<string> adjBus, adjTram, adjSprinter, adjRail;
    int id = transportNetwork.stationExistByName(stationName);
    for (int i=0; i<int(stationsList.size()); i++){
        if (stationsList[i]->getId() == id)
            continue;
        else{
            string adjStationName = stationsList[i]->getName();
            if (transportNetwork.shortestPath(adjStationName,bus)[id] != INF){
                adjBus.push_back(adjStationName);
            }
            if (transportNetwork.shortestPath(adjStationName,tram)[id] != INF){
                adjTram.push_back(adjStationName);
            }
            if (transportNetwork.shortestPath(adjStationName,sprinter)[id] != INF){
                adjSprinter.push_back(adjStationName);
            }
            if (transportNetwork.shortestPath(adjStationName,rail)[id] != INF){
                adjRail.push_back(adjStationName);
            }
        }
    }
    printAccessibleStationsInbound(adjBus,"bus");
    printAccessibleStationsInbound(adjTram,"tram");
    printAccessibleStationsInbound(adjSprinter,"sprinter");
    printAccessibleStationsInbound(adjRail,"rail");
}

/* This function prints the accessible stations to a specific station (inbound auxiliary function) */
void PublicTransportNetwork::printAccessibleStationsInbound(vector<string> &adjVehicle, string type) {
    if (adjVehicle.size() == 0){
        cout << type << ": no inbound travel\n";
    }
    else{
        cout << type << ": ";
        for (int i=adjVehicle.size()-1; i >= 0; i--){
            cout << adjVehicle[i] << "\t";
        }
        cout << "\n";
    }
}

/* Query number four - the fastest way between two stations in only one type of vehicle. Provides all the vehicles types options */
void PublicTransportNetwork::uniExpress(string &sourceStation, string &targetStation) {
    int targetId = transportNetwork.stationExistByName(targetStation);
    pair<int,int> resBus = transportNetwork.shortestPathToSpecificTarget(sourceStation,targetId,bus);
    pair<int,int> resTram = transportNetwork.shortestPathToSpecificTarget(sourceStation,targetId,tram);
    pair<int,int> resSprinter = transportNetwork.shortestPathToSpecificTarget(sourceStation,targetId,sprinter);
    pair<int,int> resRail = transportNetwork.shortestPathToSpecificTarget(sourceStation,targetId,rail);
    if (resBus.first == 0)
        cout << "bus: route unavailable\n";
    else cout << "bus: " << resBus.first + (resBus.second * stopTimes[bus]) << "\n";
    if (resTram.first == 0)
        cout << "tram: route unavailable\n";
    else cout << "tram: " << resTram.first + (resTram.second * stopTimes[tram]) << "\n";
    if (resSprinter.first == 0)
        cout << "sprinter: route unavailable\n";
    else cout << "sprinter: " << resSprinter.first + (resSprinter.second * stopTimes[sprinter]) << "\n";
    if (resRail.first == 0)
        cout << "rail: route unavailable\n";
    else cout << "rail: " << resRail.first + (resRail.second * stopTimes[rail]) << "\n";
}

/* Query number five - the fastest way between two stations in uses all the types of vehicles, include exchanges between them */
void PublicTransportNetwork::multiExpress(string &sourceStation, string &targetStation) {
    vector<pair<vector<string>, int> > results;
    int targetId = transportNetwork.stationExistByName(targetStation);
    Graph multiGraph;
    multiGraph = this->transportNetwork;
    multiGraph.multiExpressMixVehicles();
    results = multiGraph.shortestPathMultiExpress(sourceStation,targetId,stopTimes,transitTimes);
    if (results.size() == 0){
        cout << "route unavailable" << endl;
    }
    else printPath(results);
}

/* This function gets the fastest way between two stations and prints details about it (multiExpress auxiliary function) */
void PublicTransportNetwork::printPath(vector<pair<vector<string>, int> > results) {
    int stopsOrWaitingTime[results.size()], totalTime = 0;
    stopsOrWaitingTime[results.size()-1] = 0;
    stopsOrWaitingTime[0] = 0;
    for (int i=int(results.size())-1; i>=0 ; i--){
        if (i != int(results.size()) - 1 && i != 0) {
            if (results[i].first[1].compare("bus") == 0) {
                if (results[i - 1].first[1].compare("bus") == 0)
                    stopsOrWaitingTime[i] = stopTimes[bus];
                else {
                    if (results[i].first[2].compare("stad") == 0)
                        stopsOrWaitingTime[i] = transitTimes[stad];
                    else if (results[i].first[2].compare("centraal") == 0)
                        stopsOrWaitingTime[i] = transitTimes[centraal];
                    else if (results[i].first[2].compare("intercity") == 0)
                        stopsOrWaitingTime[i] = transitTimes[intercity];
                }
            }
            if (results[i].first[1].compare("tram") == 0) {
                if (results[i - 1].first[1].compare("tram") == 0)
                    stopsOrWaitingTime[i] = stopTimes[tram];
                else {
                    if (results[i].first[2].compare("stad") == 0)
                        stopsOrWaitingTime[i] = transitTimes[stad];
                    else if (results[i].first[2].compare("centraal") == 0)
                        stopsOrWaitingTime[i] = transitTimes[centraal];
                    else if (results[i].first[2].compare("intercity") == 0)
                        stopsOrWaitingTime[i] = transitTimes[intercity];
                }
            }
            if (results[i].first[1].compare("sprinter") == 0) {
                if (results[i - 1].first[1].compare("sprinter") == 0)
                    stopsOrWaitingTime[i] = stopTimes[sprinter];
                else {
                    if (results[i].first[2].compare("stad") == 0)
                        stopsOrWaitingTime[i] = transitTimes[stad];
                    else if (results[i].first[2].compare("centraal") == 0)
                        stopsOrWaitingTime[i] = transitTimes[centraal];
                    else if (results[i].first[2].compare("intercity") == 0)
                        stopsOrWaitingTime[i] = transitTimes[intercity];
                }
            }
            if (results[i].first[1].compare("rail") == 0) {
                if (results[i - 1].first[1].compare("rail") == 0)
                    stopsOrWaitingTime[i] = stopTimes[rail];
                else {
                    if (results[i].first[2].compare("stad") == 0)
                        stopsOrWaitingTime[i] = transitTimes[stad];
                    else if (results[i].first[2].compare("centraal") == 0)
                        stopsOrWaitingTime[i] = transitTimes[centraal];
                    else if (results[i].first[2].compare("intercity") == 0)
                        stopsOrWaitingTime[i] = transitTimes[intercity];
                }
            }
        }
    }
    for (int i=results.size()-1; i>=0 ; i--){
        cout << "station "<< results.size()-i << ": " << results[i].first[0] << endl;
        if (i != 0){
            totalTime += results[i-1].second;
            cout << "\tvehicle: " << results[i-1].first[1];
            cout << "    travel time: " << results[i-1].second-stopsOrWaitingTime[i];
            if (stopsOrWaitingTime[i] != 0)
                cout << "    waiting/transit time: " << stopsOrWaitingTime[i] << endl;
            else cout << endl;
        }
    }
    cout << "\ntotal time: " << totalTime << endl;
}

/* Query number six - prints all the transport network to an external File */
void PublicTransportNetwork::print() {
    this->transportNetwork.printGraph(this->outputFile);
}

/* Auxiliary function for a string parse. */
vector<string> PublicTransportNetwork::parseString(const string &s, const string& del) {
    int start = 0;
    int end = s.find(del);
    vector<string> content;
    while (end != -1) {
        content.push_back(s.substr(start, end - start));
        start = end + del.size();
        end = s.find(del, start);
    }
    content.push_back(s.substr(start, end - start));
    return content;
}