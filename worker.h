#pragma once

#include <fstream>
#include <exception>
#include <string>
#include <vector>

namespace wkfw {

	class NoResultException : public std::exception {
	public:
		NoResultException() { save(); }

		const char* what() const throw() override { return "No result given!"; }

		void save() {
			std::ofstream output;
			output.open("out.txt", std::ios::in | std::ios::app);
			output << "NoResultException: No result given!" << std::endl;
			output.close();
		}
	};

	class WorkerExecuteException : public std::exception {
 	public:
		WorkerExecuteException(const std::string& description)
			: description(description) { save(); }

		const char* what() const throw() override { return description.c_str(); }

		void save(){
			std::ofstream output;
			output.open("out.txt", std::ios::in | std::ios::app);
			output << description << std::endl;
			output.close();
		}

 	private:
 	 	const std::string description;
	};

	class WorkerResult {
	public:
		enum ResultType { UNKNOWN, NONE, TEXT };
		WorkerResult() : type(NONE) {}

		WorkerResult(ResultType type) : type(type) {}

		WorkerResult(const std::vector<std::string>& value) : type(TEXT), value(value) {}

		WorkerResult(const WorkerResult& result) : type(result.type), value(result.value) {}

		WorkerResult& operator=(const WorkerResult& from) {
			type = from.type;
			value = from.value;
			return (*this);
		}

		bool operator==(const WorkerResult& other) const {
			return type == other.type && value == other.value;
		}

		bool operator!=(const WorkerResult& other) const {
			return type != other.type || value != other.value;
		}

		const ResultType getType() const { return type; }

		const std::vector<std::string> getValue() const {
			try {
				if (type == NONE)
					throw NoResultException();
				return value;
			}
			catch (NoResultException obj) {
				std::vector<std::string> _null_str_vect;
				return _null_str_vect;
			}
		}
	private:
		ResultType type;
		std::vector<std::string> value;
	};

	class Worker {
	public:
		Worker(const size_t identifier,
					WorkerResult::ResultType returnType,
					WorkerResult::ResultType acceptType)
				: identifier(identifier),
					returnType(returnType),
					acceptType(acceptType) {}

		virtual const WorkerResult execute(const WorkerResult& previous) const  = 0;

		size_t getId() const { return identifier; }

		WorkerResult::ResultType getReturnType() const { return returnType; }

		WorkerResult::ResultType getAcceptType() const { return acceptType; }

		int isValidAcceptedResult(const WorkerResult& result) const{
			if (result.getType() == acceptType)
				return 1;
			else {
				WorkerExecuteException exeption("Isn't valid accepted type of\
																previous result");
				return 0;
			}
		}

		virtual ~Worker() {}

	private:
		const WorkerResult::ResultType returnType;
		const WorkerResult::ResultType acceptType;
		const size_t identifier;
	};

}  
