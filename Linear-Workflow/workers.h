#pragma once
#include <string>

#include "worker.h"

namespace workers {
  using namespace wkfw;

  const Worker* constructWorker(const size_t ident,
                                const std::string& name,
                                const std::vector<std::string>& args);

  class ReadFile : public Worker {
  public:
    ReadFile(const size_t ident, const std::string& filename): Worker(ident,
                                            WorkerResult::ResultType::TEXT,
                                            WorkerResult::ResultType::NONE),
                                            filename(filename) {}

    const WorkerResult execute(const WorkerResult& previous) const override;

  private:
    const std::string filename;
  };

  class WriteFile : public Worker {
  public:
    WriteFile(const size_t ident, const std::string& filename): Worker(ident,
                                              WorkerResult::ResultType::NONE,
                                              WorkerResult::ResultType::TEXT),
                                              filename(filename) {}

    virtual const WorkerResult execute(const WorkerResult& previous) const override;

  protected:
    WriteFile(const size_t ident,
        const std::string& filename,
        WorkerResult::ResultType returnType): Worker(ident,
                                              returnType,
                                              WorkerResult::ResultType::TEXT),
                                              filename(filename) {}

  private:
    const std::string filename;
  };

  class Grep : public Worker {
  public:
    Grep(const size_t ident, const std::string& pattern): Worker(ident,
                                              WorkerResult::ResultType::TEXT,
                                              WorkerResult::ResultType::TEXT),
                                              pattern(pattern) {}

    const WorkerResult execute(const WorkerResult& previous) const override;

  private:
    const std::string pattern;
  };

  class Sort : public Worker {
  public:
    Sort(const size_t ident): Worker(ident,
                              WorkerResult::ResultType::TEXT,
                              WorkerResult::ResultType::TEXT) {}

    const WorkerResult execute(const WorkerResult& previous) const override;
  };

  class Replace : public Worker {
  public:
    Replace(const size_t ident,
            const std::string& pattern,
            const std::string& substitution): Worker(ident,
                                              WorkerResult::ResultType::TEXT,
                                              WorkerResult::ResultType::TEXT),
                                              pattern(pattern),
                                              substitution(substitution) {}

    const WorkerResult execute(const WorkerResult& previous) const override;

  private:
    const std::string pattern;
    const std::string substitution;
  };

  class Dump : public WriteFile {
  public:
    Dump(const size_t ident,
         const std::string& filename): WriteFile(ident,
                                                  filename,
                                                 WorkerResult::ResultType::TEXT) {}

    const WorkerResult execute(const WorkerResult& previous) const override;

  private:
    const std::string filename;
  };
}
