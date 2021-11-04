#include "workflow.h"

namespace wkfw{
	Workflow::Workflow(const std::string& ifname, std::string ofname): 
	ifname(ifname),
    ofname(ofname) {
		if (ofname == "") {
			std::cout << "No output file set. Redirecting the stream in \"out.txt\"" << std::endl;
			ofname = "out.txt";
		}
		if(ifname != ""){
			WorkflowParser filenameParser(ifname);
			if(filenameParser.isInputValid){
				parser = filenameParser;
				buildDescription();
			}
			else 
				isEmpty = true;			
		}  
		else isEmpty = true;      
	}

	Workflow::Workflow(const Workflow& other){
		isEmpty = !other;
		ifname = other.ifname;
		ofname = other.ofname;
		if(other){
			WorkflowParser filenameParser(ifname);
			parser = filenameParser;
			buildDescription();
		}
	}	
	
	const Worker* Workflow::getWorkerById(uint32_t id){
		auto found = description.find(id);
		return found == description.end() ? nullptr : found->second;
	}

	const Worker* Workflow::nextInstruction() {
	const Worker* worker = (!parser.instruction.empty()
			 ? getWorkerById(parser.instruction.front())               
			 : nullptr);
	if(parser.instruction.size())
		parser.instruction.pop_front();
	return worker;
	}

	const std::string Workflow::extractCmd(std::string cmd_line){
		if(cmd_line.find(" "))
			return cmd_line.substr(0, cmd_line.find(" "));
		else
			return cmd_line;		
	}

	const std::vector<std::string> Workflow::extractArgs(std::string cmd_line){
		std::vector<std::string> args;
		if(cmd_line.find(" ") > 1 && cmd_line.find(" ") < cmd_line.length()) {
			size_t pointer = cmd_line.find(" ") + 1;
			if (cmd_line.find(" ", pointer)) {
				args.push_back(cmd_line.substr(pointer, cmd_line.find(" ", pointer)- pointer));
				pointer = cmd_line.find(" ", pointer) + 1;
				if(pointer)
					args.push_back(cmd_line.substr(pointer, cmd_line.length() - pointer));
			}
		}      
		return args;
	}


	void Workflow::reciveCmd(uint32_t id, const std::string cmd_line) {
		const std::string cmd = extractCmd(cmd_line);
		const std::vector<std::string> args = extractArgs(cmd_line);
		if (!isEmpty) {
			const Worker* worker = workers::constructWorker(id, cmd, args);
			description[id] = worker;			
		}
		if (isEmpty)
			WorkflowException(">" + std::to_string(id) + ": " + cmd_line);
	}
	void Workflow::buildDescription(){
		for(uint32_t i = 0; i < parser.blocks.size(); ++i)
			if(!isEmpty)
				reciveCmd(i, parser.blocks[i]);
	}

	void Workflow::run(void){
		if(!isEmpty){
			WorkerResult lastResult;
			Worker const* worker = nextInstruction();
			if(worker == nullptr){
				isEmpty = true;
				return;
			}
			if(worker->getAcceptType() != WorkerResult::NONE){
				workers::ReadFile reader(0, ifname);
				lastResult = reader.execute(lastResult);
			}

			do {
				lastResult = worker->execute(lastResult);
			}
			while((worker = nextInstruction()));

			if (lastResult.getType() != WorkerResult::NONE) {
				workers::WriteFile writer(0, ofname);
				writer.execute(lastResult);
			}
		}
		else{
			std::ofstream output;
			output.open(ofname.c_str());
			output << parser.exception << "Error: Empty workflow";
			output.close();
		}
	}

	
}