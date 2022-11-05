#pragma once

#include "person.hpp"

#include <string>
#include <iostream>
#include <fstream>

/*
*/

class PeopleNode
{
    PeopleNode();
    PeopleNode(Person *toAdd);
    ~PeopleNode();

    Person      *person; //the data
    PeopleNode  *next; //the pointer
    friend class People; //because
};


class People
{
    public:
        People();
        ~People();
        int populate(std::string file_in, char type_in);
        bool add_person(Person *to_add);
        Person * find_person(int num_to_find);
        //bool remove_person(int num_to_remove);
        bool write_out(std::string file_out);

    protected:
        PeopleNode *head;
};