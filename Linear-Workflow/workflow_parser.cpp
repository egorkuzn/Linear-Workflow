#include "workflow_parser.h"

namespace wkfw{        
        WorkflowParcer::WorkflowParcer(const std::string& filename){
            getFromFile(filename);
        }

        void WorkflowParcer::operator=(WorkflowParcer other){
            isInputValid = other.isInputValid;
            instruction = other.instruction;
            blocks = other.blocks;
        }

        int WorkflowParcer::getInstruction(std::string& line) {
            uint32_t count = 0;
            for (size_t i = 0; i < line.length(); ++i) {
                if ((0 <= line[i] && line[i] <= '9') || (line[i] == '-' && line[i + 1] == '>')\
                    || (i > 0 && line[i - 1] == '-' && line[i] == '>')) {
                    if (line[i] == '>') {
                        instruction.resize(instruction.size() + 1);
                    }
                    else {
                        if (line[i] == '-') {
                            instruction.push_back(count);
                            count = 0;
                        }
                        else {
                            count *= 10;
                            count += line[i] - '0';
                            if (i = line.length() - 1)
                                instruction.push_back(count);
                        }
                    }
                }
                else
                    return 0;
            }
            return 1;
        }

    int WorkflowParcer::getBlock(std::string& line) {
        if (line.find(" ")) {
            size_t pointer = line.find(" ");
            std::string sub_id = line.substr(0, pointer);
            pointer += 3;
            std::string sub_cmd = line.substr(pointer, line.length() - pointer);
            uint32_t id = std::stoul(sub_id);
            if (blocks.capacity() < id+1)
                blocks.resize(id+1);
            blocks[id] = sub_cmd;
            return 1;
        }
        else
            return 0;
    }

    void WorkflowParcer::getFromFile(const std::string& filename) {
        std::string line;
        std::ifstream input;
        try{            
            input.open(filename.c_str()); 
            if (!input)
                throw("problem in opening");
            getline(input, line);
            if(line.find("desc", 0, 1))
                throw ("desc not found");
            getline(input, line);
            while((!input.eof()) && (line.find("csed", 0, 1))){
                if(!getBlock(line))
                    throw("no _split_ in the cmd string block");
                getline(input, line);
            }
            if(line.find("csed", 0, 1))
                throw("csed not found");
            getline(input, line);
            if (!line.length())
                throw("No instruction \"csed\" after");
            instruction.resize(1);
            if (!getInstruction(line))
                throw("bad char in the sorce file, when instruction reading");
            input.close();
        } catch(std::istream::failure& e){
            WorkflowException("Problem with reading and opening \"" + filename + "\"");
            blocks.clear();
            isInputValid = false;
            input.close();
        } catch(const char* msg){
            WorkflowException(msg);
            blocks.clear();
            isInputValid = false;
            input.close();
        } catch(const std::invalid_argument& ia){
            WorkflowException("Invalid argument");
            blocks.clear();
            isInputValid = false;
            input.close();
        }

    }

}