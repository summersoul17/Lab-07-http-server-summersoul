#include <suggestion.hpp>

bool comp(const sug_config& first, const sug_config& second) {
  return first.cost > second.cost;
}

json suggestion::get_json_suggestion(string input) {
  json j = json::parse(input);
  std::vector<json> out_suggestions;
  int count = 0;
  mutex.lock();
  std::sort(suggestions.begin(), suggestions.end(), comp);
  for (auto& sug : suggestions){
    if (sug.id == j["input"].get<string>()) {
      sug_answer answer = sug_answer{sug.name, ++count};
      out_suggestions.push_back(json{{"text", answer.text},
                                    {"position", answer.position}
      });
    }
  }
  mutex.unlock();
  json out_json;
  out_json["suggestions"] = out_suggestions;
  return out_json;
}

void suggestion::update_suggestion() {
  for (;;){
    mutex.lock();
    std::ifstream file(file_name_);
    suggestions.clear();
    data.clear();
    if (file_name_.empty()){
      throw std::invalid_argument("empty file\n");
    }
    if (!file) {
      throw std::runtime_error("bad open json_file: " + file_name_);
    }
    file >> data;
    file.close();
    for (auto const& sug : data.at("suggestions")){
      suggestions.push_back({
          sug["id"].get<string>(),
              sug["name"].get<string>(),
                  sug["cost"].get<int>()
                      });
    }
    mutex.unlock();
    sleep(40);
  }
}
