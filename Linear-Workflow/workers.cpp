#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>

#include "workers.h"

namespace workers {
  using namespace wkfw;

  const Worker* constructWorker(const size_t ident,
                                      const std::string& name,
                                      const std::vector<std::string>& args) {
    if (name == "readfile") {
      if (args.size() == 1)
        return new ReadFile(ident, args[0]);
    } else if (name == "writefile") {
      if (args.size() == 1)
        return new WriteFile(ident, args[0]);
    } else if (name == "grep") {
      if (args.size() == 1)
        return new Grep(ident, args[0]);
    } else if (name == "sort") {
      if (args.size() == 0)
        return new Sort(ident);
    } else if (name == "replace") {
      if (args.size() == 2)
        return new Replace(ident, args[0], args[1]);
    } else if (name == "dump") {
      if (args.size() == 1)
        return new Dump(ident, args[0]);
    }

    return nullptr;
  }

  const WorkerResult ReadFile::execute(const WorkerResult& previous)
    const {
    std::ifstream input;
    std::vector<std::string> list;

    input.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    if (previous.getType() != WorkerResult::NONE)
        list = previous.getValue();

    try {
      input.open(filename);
      std::string line;
      while (!input.eof() && std::getline(input, line))
        list.push_back(line);
      input.close();
    } catch (std::ifstream::failure& e) {
      if (!input.eof()){
        throw wkfw::WorkerExecuteException("Cannot read lines from file \"" +
                                         filename + "\"");        
      }
    }

    return WorkerResult(list);
  }

  const WorkerResult WriteFile::execute(const WorkerResult& previous) const {
    if (!isValidAcceptedResult(previous))
      return WorkerResult(WorkerResult::UNKNOWN);

    std::ofstream output;

    output.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
      output.open(filename, std::ios::in | std::ios::app);    
      for (auto const& line : previous.getValue())
        output << line << std::endl;
      output.close();
    } catch (std::ofstream::failure& e) {
      throw WorkerExecuteException("Cannot write lines to file \"" +
                                       filename + "\"");
      return WorkerResult(WorkerResult::UNKNOWN);
    } 
    return WorkerResult();
  }

  const WorkerResult Grep::execute(const WorkerResult& previous) const {
    if (!isValidAcceptedResult(previous))
      return WorkerResult(WorkerResult::UNKNOWN);

    std::vector<std::string> list;

    for (auto const& line : previous.getValue())
      if (line.find(pattern))
        list.push_back(line);

    return WorkerResult(list);
  }

  const WorkerResult Sort::execute(const WorkerResult& previous) const {
    if (!isValidAcceptedResult(previous))
      return WorkerResult(WorkerResult::UNKNOWN);

    std::vector<std::string> list = previous.getValue();

    std::sort(list.begin(), list.end());

    return WorkerResult(list);
  }

  static std::string replace(const std::string& str,
                            const std::string& pattern,
                            const std::string& substitution) {
    std::string result(str);
    size_t index = 0;
    while (true) {
      index = str.find(pattern, index);
      if (index == std::string::npos)
        break;
      result.replace(index, pattern.size(), substitution);
      index += substitution.size();
    }
    return result;
  }

  const WorkerResult Replace::execute(const WorkerResult& previous) const {
    if (!isValidAcceptedResult(previous))
      return WorkerResult(WorkerResult::UNKNOWN);

    std::vector<std::string> list;

    for (auto const& line : previous.getValue())
      list.push_back(replace(line, pattern, substitution));

    return WorkerResult(list);
  }

  const WorkerResult Dump::execute(const WorkerResult& previous) const {
    if (!isValidAcceptedResult(previous))
      return WorkerResult(WorkerResult::UNKNOWN);

    WriteFile::execute(previous);

    return WorkerResult(previous.getValue());
  }

}
