
#include "Arguments.hpp"
#include <iostream>


nlohmann::json  Arguments::to_json() {

  nlohmann::json obj;
  obj["method"] = _name;
  obj["arguments"] = json::object();

  return obj;
}


std::string Arguments::to_json_string() {
  return to_json().dump();
};


ArgumentFactory::EndGame::EndGame() {
  _name = "end_game";
};

ArgumentFactory::EndGame::EndGame(nlohmann::json& arguments) {
  _name = arguments["method"];
};


ArgumentFactory::Back::Back() {
  _name = "back";
};

ArgumentFactory::Back::Back(nlohmann::json& arguments) {

  _name = arguments["method"];
  _color = arguments["arguments"]["color"];
  _position = arguments["arguments"]["position"];
  _captured = std::vector<std::string>();

  json& j_cap = arguments["arguments"]["captured"];
  for (json::iterator it = j_cap.begin(); it != j_cap.end(); ++it) {
    _captured.push_back(*it);
  }

};

nlohmann::json  ArgumentFactory::Back::to_json() {

  nlohmann::json obj = Arguments::to_json();

  nlohmann::json& arguments = obj["arguments"];
  arguments["color"] = _color;
  arguments["position"] = _position;
  arguments["captured"] = _captured;

  return obj;
};


ArgumentFactory::MakeTurn::MakeTurn() {
  _name = "make_turn";
};

ArgumentFactory::MakeTurn::MakeTurn(nlohmann::json& arguments) {

  _name = arguments["method"];
  _color = arguments["arguments"]["color"];
  _position = arguments["arguments"]["position"];
  _captured = std::vector<std::string>();
  _hints = std::vector<std::string>();


  json& j_cap = arguments["arguments"]["captured"];
  for (json::iterator it = j_cap.begin(); it != j_cap.end(); ++it) {
    _captured.push_back(*it);
  }

  json& j_hints = arguments["arguments"]["hints"];
  for (json::iterator it = j_hints.begin(); it != j_hints.end(); ++it) {
    _hints.push_back(*it);
  }

};

nlohmann::json  ArgumentFactory::MakeTurn::to_json() {

  nlohmann::json obj = Arguments::to_json();

  nlohmann::json& arguments = obj["arguments"];
  arguments["color"] = _color;
  arguments["position"] = _position;
  arguments["captured"] = _captured;
  arguments["hints"] = _hints;

  obj["arguments"] = arguments;

  return obj;
};


ArgumentFactory::Hints::Hints() {
  _name = "print_hints";
};

ArgumentFactory::Hints::Hints(nlohmann::json& arguments) {

  _name = arguments["method"];
  _hints = std::vector<std::string>();

  json& j_hints = arguments["arguments"]["hints"];
  for (json::iterator it = j_hints.begin(); it != j_hints.end(); ++it) {
    _hints.push_back(*it);
  }

};

nlohmann::json  ArgumentFactory::Hints::to_json() {

  nlohmann::json obj = Arguments::to_json();

  nlohmann::json& arguments = obj["arguments"];
  arguments["hints"] = _hints;

  obj["arguments"] = arguments;

  return obj;
};


ArgumentFactory::Winner::Winner() {
  _name = "winner";
};

ArgumentFactory::Winner::Winner(nlohmann::json& arguments) {
  _name = arguments["method"];
  _winner = arguments["arguments"]["winner"];
};

nlohmann::json  ArgumentFactory::Winner::to_json() {

  nlohmann::json obj = Arguments::to_json();

  nlohmann::json& arguments = obj["arguments"];
  arguments["winner"] = _winner;

  obj["arguments"] = arguments;

  return obj;
};
