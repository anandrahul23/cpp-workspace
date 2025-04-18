
#include "Request.h"
IRequest::~IRequest()
{

}

IRequest::IRequest():internal_direction{Direction::UP}, source_floor{0}, destination_floor{0}
{

}

ExternalRequest::ExternalRequest(/* args */)
{
}

ExternalRequest::~ExternalRequest()
{
}


InternalRequest::InternalRequest(/* args */)
{
}

InternalRequest::~InternalRequest()
{
}