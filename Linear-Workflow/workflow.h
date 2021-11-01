#pragma once
#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <map>

#include "workflow_parser.h"
#include "worker.h"
#include "workers.h"

namespace wkfw{
    class Workflow{
     public:
        Workflow(const std::string& ifname, const std::string& ofname);

        Workflow(const Workflow& other);

        ~Workflow(){}

        operator bool() const{
            return !isEmpty;
        }

        const Worker* nextInstruction(void);
        void run(void);
        std::string ifname;
        std::string ofname;
    private:        
        std::map<uint32_t, const Worker*> description;
        WorkflowParcer parser;
        bool isEmpty = false; 
        const Worker* getWorkerById(uint32_t id);
        const std::string extractCmd(std::string cmd_line);
        const std::vector<std::string> extractArgs(std::string cmd_line);
        void reciveCmd(uint32_t id, std::string cmd);
        void buildDescription();
        void WorkflowException(std::string msg) {
            std::cout << "Workflow exception: " << msg << std::endl;
        }
    };
}
 