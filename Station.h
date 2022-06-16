#ifndef EX2_STATION_H
#define EX2_STATION_H

#include <iostream>
using namespace std;
enum typeOfStation {intercity, centraal, stad};
enum typeOfVehicle {bus,tram,sprinter,rail,general};

/* This class represent a station in the transport network */
/* I used the defaults 'the big three' except constructor */
class Station {
private:
    string name;
    typeOfStation stationType;
    typeOfVehicle vehicleType;
    int id;

public:
    Station(string &name, typeOfStation st, typeOfVehicle vt);
    int getId() const;
    string getName() const;
    void setTypeOfVehicle(typeOfVehicle type);
    typeOfStation getTypeOfStation() const;
    typeOfVehicle getTypeOfVehicle() const;
    static int totalStations;
};

#endif //EX2_STATION_H