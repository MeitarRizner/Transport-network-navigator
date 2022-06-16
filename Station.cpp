#include "Station.h"

/* constructor */
Station::Station(string &name, typeOfStation st, typeOfVehicle vt) : name(name), stationType(st), vehicleType(vt), id(totalStations){
    totalStations++;
}
int Station::totalStations = 0;

/* getter - id */
int Station::getId() const{
    return this->id;
}

/* getter - name */
string Station::getName() const {
    return this->name;
}

/* getter - type of station */
typeOfStation Station::getTypeOfStation() const {
    return this->stationType;
}

/* setter - type of station */
void Station::setTypeOfVehicle(typeOfVehicle type) {
    this->vehicleType = type;
}

/* getter - type of vehicle */
typeOfVehicle Station::getTypeOfVehicle() const {
    return this->vehicleType;
}