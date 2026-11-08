#pragma once
#include <ostream>
#include <string>

class IContainer {
public:
    virtual ~IContainer() = default;
    virtual char type() const = 0;                  
    virtual void print(std::ostream& os) const = 0; 
    virtual std::string serialize() const = 0;      
    virtual void deserialize(const std::string&) = 0;
};