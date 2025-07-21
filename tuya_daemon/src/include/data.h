#ifndef DATA_H
#define DATA_H

struct InterfacesData {
    char name[50];
    char ip[50];
    int mask;
};

struct Data {
    int total_ram;
    int free_ram;
    struct InterfacesData *interfaces;
    int count_ifa;
};

#endif
