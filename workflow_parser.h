#pragma once

#include <fstream>
#include <string>
#include <list>
#include <vector>
#include <iostream>

namespace wkfw{
    class WorkflowParser{
        public:
            WorkflowParser(const std::string& filename);
            WorkflowParser(){}

            void operator=(WorkflowParser other);

            WorkflowParser(const WorkflowParser& other){
                instruction = other.instruction;
                blocks = other.blocks;
                isInputValid = other.isInputValid;
                exception = other.exception;
            }
            std::string exception;
            std::list<uint32_t> instruction;
            std::vector<std::string> blocks; 
            bool isInputValid = true;
        private:
            int getInstruction(std::string& line);
            std::vector<bool> blocksRange;
            void WorkflowParserException(std::string msg) {
                isInputValid = false;
                exception += "WorkflowParser exception: " + msg + "\n";
            }
            int getBlock(std::string& line);
            void getFromFile(const std::string& filename);
            int deleteWhiteSpaces(std::string& line);
            int addElemInstruction(uint32_t& count, bool& isLastCount);
            void dataOptimize(void);
            void deleteTwins(void);
            void deleteNoUsedBlocks();
    };
}