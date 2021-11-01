#pragma once

#include <exception>
#include <string>
#include <vector>

namespace wkfw {

	class NoResultException : public std::exception {
	public:
		const char* what() const throw() override { return "No result given!"; }
	};

	class WorkerExecuteException : public std::exception {
 	public:
  		WorkerExecuteException(const std::string& description)
      	: description(description) {}

 	 	const char* what() const throw() override { return description.c_str(); }

 	private:
 	 	const std::string description;
	};

	class WorkerResult {
	public:
		enum ResultType { UNKNOWN, NONE, TEXT };
		WorkerResult() : type(NONE) {}

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
			if (type == NONE)
				throw NoResultException();
			return value;
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

		virtual ~Worker() {}

	private:
		const WorkerResult::ResultType returnType;
		const WorkerResult::ResultType acceptType;
		const size_t identifier;
	};

}  
