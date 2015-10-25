#ifndef UUID_H
#define UUID_H

#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

#define generate_uuid() boost::uuids::to_string( boost::uuids::random_generator()() )

#endif //UUID_H
