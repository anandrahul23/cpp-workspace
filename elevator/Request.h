#ifndef _REQUEST_H_
#define _REQUEST_H_

#include "ElevatorContstant.h"
class IRequest
{
    private:
     /* data */
    int source_floor;
    int destination_floor;
    Direction internal_direction;
    public: 
    virtual ~IRequest() =0;
    IRequest();
    int getSourceFloor() const
    {
        return source_floor;
    }
    int getDestinationFloor() const
    {
        return destination_floor;
    }

};



class ExternalRequest : public IRequest
{
private:

public:
    ExternalRequest(/* args */);
    ~ExternalRequest();
};


class InternalRequest: public IRequest
{
private:
   
public:
    InternalRequest(/* args */);
    ~InternalRequest();
};

#endif



