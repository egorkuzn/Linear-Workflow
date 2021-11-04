#include "workflow_parser.h"

namespace wkfw{        
        WorkflowParser::WorkflowParser(const std::string& filename){
            getFromFile(filename);
        }

        void WorkflowParser::operator=(WorkflowParser other){
            isInputValid = other.isInputValid;
            instruction = other.instruction;
            blocks = other.blocks;
        }

        int WorkflowParser::deleteWhiteSpaces(std::string& line) {
            std::string lineWithoutSpaces;
            bool isLastCount = true;
            for (size_t i = 0; i < line.length(); ++i) {
                if (line[i] == '>' || line[i] == '-') {
                    isLastCount = false;
                    lineWithoutSpaces.push_back(line[i]);                    
                }
                else if ('0' <= line[i] && line[i] <= '9') {
                isLastCount = true;
                lineWithoutSpaces.push_back(line[i]);
                }
                else if (line[i] == ' ' 
                        && (isLastCount || (i > 0)
                        && (line[i - 1] == '>'
                            || line[i - 1] == ' ') ) )
                    continue;
                else
                    return 0;                
            }
            line = lineWithoutSpaces;
        }

        int WorkflowParser::addElemInstruction(uint32_t count, bool isLastCount) {
            if (count < blocksRange.capacity() && blocksRange[count]) {
                instruction.push_back(count);
                count = 0;
                isLastCount = false;
            }
            else {
                WorkflowParserException("bad id of instruction");
                return 0;
            }
        }

        int WorkflowParser::getInstruction(std::string& line) {
            if (!deleteWhiteSpaces(line))
                return 0;

            uint32_t count = 0;
            bool isLastCount = false;

            for (size_t i = 0; i < line.length(); ++i) {
                if (('0' <= line[i] && line[i] <= '9')
                    || ((i > 0) && line[i - 1] == '-'
                                && line[i] == '>')
                    || (((i + 1)< line.length())
                        && line[i] == '-'
                        && line[i + 1] == '>') ) {
                    if (line[i] == '>' && isLastCount) {
                        if (!addElemInstruction(count, isLastCount))
                            return 0;
                    }
                    else if (line[i] != '-') {
                        isLastCount = true;
                        count *= 10;
                        count += line[i] - '0';                   
                    }
                }
                else {
                    instruction.clear();
                    return 0;
                }
            }

            if (isLastCount && addElemInstruction(count, isLastCount)) {
                return 1;
            }
            else {
                instruction.clear();
                return 0;
            }
        }

    int WorkflowParser::getBlock(std::string& line) {
        if (!line.length())
            return 0;
        if (line.find(" ")) {
            size_t pointer = line.find(" ");
            std::string sub_id = line.substr(0, pointer);
            pointer += 3;
            std::string sub_cmd = line.substr(pointer, line.length() - pointer);
            uint32_t id = std::stoul(sub_id);

            if (blocks.capacity() < id + 1) {
                blocksRange.resize(id + 1);
                blocks.resize(id + 1);
            }

            blocksRange[id] = true;
            blocks[id] = sub_cmd;
            return 1;
        }
        else
            return 0;
    }

    void WorkflowParser::getFromFile(const std::string& filename) {
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
                    throw("no _split_ in the cmd string block, or source line is empty");
                getline(input, line);
            }

            if(line.find("csed", 0, 1))
                throw("csed not found");

            getline(input, line);

            if (!line.length())
                throw("No instruction \"csed\" after");

            if (!getInstruction(line))
                throw("bad char in the source file, when instruction reading");

            input.close();
        } catch(std::istream::failure& e){
            WorkflowParserException("Problem with reading and opening \"" + filename + "\"");
            blocks.clear();
            isInputValid = false;
            input.close();
        } catch(const char* msg){
            WorkflowParserException(msg);
            blocks.clear();
            isInputValid = false;
            input.close();
        } catch(const std::invalid_argument& ia){
            WorkflowParserException("Invalid argument");
            blocks.clear();
            isInputValid = false;
            input.close();
        }

    }

}