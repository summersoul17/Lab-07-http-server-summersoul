#ifndef INCLUDE_SUGGESTION_HPP_
#define INCLUDE_SUGGESTION_HPP_
#include <shared_mutex>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <unistd.h>

using json = nlohmann::json;
using string = std :: string;

struct sug_config{
  string id;
  string name;
  int cost;
};

struct sug_answer{
  string text;
  int position;
};

class suggestion {
 public:
  explicit suggestion(string file_name) : file_name_(file_name){}

  json get_json_suggestion(string input);

  void update_suggestion();

 private:
  json data;

  std::shared_mutex mutex;

  std::vector<sug_config> suggestions;

  string file_name_;
};
#endif  // INCLUDE_SUGGESTION_HPP_
