#pragma once

#include <fstream>
#include <string>
#include <list>
#include <vector>
#include <iostream>

namespace wkfw{
    class WorkflowParcer{
        public:
            WorkflowParcer(const std::string& filename);
            WorkflowParcer(){}

            void operator=(WorkflowParcer other);

            WorkflowParcer(const WorkflowParcer& other){
                instruction = other.instruction;
                blocks = other.blocks;
                isInputValid = other.isInputValid;
            }

            std::list<uint32_t> instruction;
            std::vector<std::string> blocks; 
            bool isInputValid = true;
        private:
            int getInstruction(std::string& line);
            void WorkflowException(std::string msg) {
                std::cout << "Workflow exception: " << msg << std::endl;
            }
            int getBlock(std::string& line);
            void getFromFile(const std::string& filename);
    };
}