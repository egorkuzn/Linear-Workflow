#include "workflow_parser.h"

namespace wkfw{        
        WorkflowParser::WorkflowParser(const std::string& filename){
            getFromFile(filename);
        }

        void WorkflowParser::operator=(WorkflowParser other){
            isInputValid = other.isInputValid;
            instruction = other.instruction;
            blocks = other.blocks;
            exception = other.exception;
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

        int WorkflowParser::addElemInstruction(uint32_t& count, bool& isLastCount) {
            if (count < blocksRange.size() && blocksRange[count]) {
                instruction.push_back(count);
                count = 0;        
                isLastCount = false;        
                return 1;        
            }               
            WorkflowParserException("bad id of instruction");
            return 0;            
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
                    || (((i + 1) < line.length())
                        && line[i] == '-'
                        && line[i + 1] == '>')) {
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
                else
                    return 0;
            }

            if (isLastCount && addElemInstruction(count, isLastCount)) {
                return 1;
            }
            else 
                return 0;            
        }

    int WorkflowParser::getBlock(std::string& line) {
        if (!line.length()) {
            WorkflowParserException("Empty block line");
            return 0;
        }
        if (line.find(" ") != std::string::npos) {
            size_t pointer = line.find(" ");
            std::string sub_id = line.substr(0, pointer);
            pointer += 3;
            std::string sub_cmd = line.substr(pointer, line.length() - pointer);

            if (sub_id[0] == '-') {
                WorkflowParserException("Negative id");
                return 0;
            }

            uint32_t id = std::stoul(sub_id);

            if (blocks.capacity() < id + 1) {
                blocksRange.resize(id + 1);
                blocks.resize(id + 1);
            }

            blocksRange[id] = true;
            blocks[id] = sub_cmd;
            return 1;
        }
        else {
            WorkflowParserException("No _split_ detected");
            return 0;
        }
    }

    void WorkflowParser::dataOptimize(void) {
        uint32_t i = 0;
        std::list<uint32_t> new_instruction;
        do {
            if (blocksRange[i]) {
                continue;
            }
            else {
                uint32_t iPrevios = i;

                while (!blocksRange[iPrevios] && iPrevios < blocksRange.size())
                    ++iPrevios;

                blocksRange.erase(blocksRange.begin() + i, blocksRange.begin() + iPrevios);
                blocks.erase(blocks.begin() + i, blocks.begin() + iPrevios);
                uint32_t delta = iPrevios - i;

                for (uint32_t n : instruction) {
                    if (n > i)
                        new_instruction.push_back(n - delta);
                    else
                        new_instruction.push_back(n);
                }

                instruction = new_instruction;
                new_instruction.clear();
            }                
        } while (++i < blocksRange.size());
    }

    void WorkflowParser::deleteTwins() {
        if (blocks.size() > 1) {
            std::list<uint32_t> new_intruction;
            blocksRange.assign(blocksRange.size(), false);
            for (uint32_t n : instruction) {
                for (uint32_t i = 0; i < blocks.size(); ++i)
                    if (blocks[i] == blocks[n]) {
                        n = i;
                        break;
                    }
                blocksRange[n] = true;
                new_intruction.push_back(n);
            }
            instruction = new_intruction;
        }
    }

    void WorkflowParser::deleteNoUsedBlocks() {
        uint32_t i = 0;
        do {
            if (!blocksRange[i]) {
                blocks.erase(blocks.begin() + i);
                blocksRange.erase(blocksRange.begin() + i);
                if(i > 0)
                    --i;
            }
        } while (++i < blocksRange.size());
        blocksRange.clear();
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
                    throw("invalid block string");
                getline(input, line);
            }

            if(line.find("csed", 0, 1))
                throw("csed not found");

            getline(input, line);

            if (!line.length())
                throw("No instruction \"csed\" after");

            if (!getInstruction(line))
                throw("bad char in the source file, when instruction reading");

            dataOptimize();

            deleteTwins();

            deleteNoUsedBlocks();

            input.close();
        } catch(std::istream::failure& e){
            WorkflowParserException("Problem with reading and opening \"" + filename + "\"");
            input.close();
        } catch(const char* msg){
            WorkflowParserException(msg);
            input.close();
        } catch(const std::invalid_argument& ia){
            WorkflowParserException("Invalid argument");
            input.close();
        }

    }

}