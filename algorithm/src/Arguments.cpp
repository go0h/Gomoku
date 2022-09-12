
#include "Arguments.hpp"
#include <iostream>


std::string& Arguments::getName() {
  return _name;
}

nlohmann::json Arguments::to_json() {

  nlohmann::json obj;
  obj["method"] = _name;
  obj["arguments"] = json::object();

  return obj;
}

std::string Arguments::to_json_string() {
  return to_json().dump();
};


EndGame::EndGame() {
  _name = "end_game";
};

EndGame::EndGame(nlohmann::json& arguments) {
  _name = arguments["method"];
};


Back::Back() {
  _name = "back";
};

Back::Back(nlohmann::json& arguments) {

  _name = arguments["method"];
  _color = arguments["arguments"]["color"];
  _position = arguments["arguments"]["position"];
  _captured = std::vector<std::string>();

  json& j_cap = arguments["arguments"]["captured"];
  for (json::iterator it = j_cap.begin(); it != j_cap.end(); ++it) {
    _captured.push_back(*it);
  }

};

nlohmann::json  Back::to_json() {

  nlohmann::json obj = Arguments::to_json();

  nlohmann::json& arguments = obj["arguments"];
  arguments["color"] = _color;
  arguments["position"] = _position;
  arguments["captured"] = _captured;

  return obj;
};


MakeTurn::MakeTurn() {
  _name = "make_turn";
};

MakeTurn::MakeTurn(nlohmann::json& arguments) {

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

MakeTurn::MakeTurn(std::string color,
                   std::string position,
                   std::vector<std::string> captured,
                   std::vector<std::string> hints) :
    _color(color),
    _position(position),
    _captured(captured),
    _hints(hints) {

  _name = "make_turn";
};

nlohmann::json  MakeTurn::to_json() {

  nlohmann::json obj = Arguments::to_json();

  nlohmann::json& arguments = obj["arguments"];
  arguments["color"] = _color;
  arguments["position"] = _position;
  arguments["captured"] = _captured;
  arguments["hints"] = _hints;

  obj["arguments"] = arguments;

  return obj;
};


Hints::Hints() {
  _name = "print_hints";
};

Hints::Hints(nlohmann::json& arguments) {

  _name = arguments["method"];
  _hints = std::vector<std::string>();

  json& j_hints = arguments["arguments"]["hints"];
  for (json::iterator it = j_hints.begin(); it != j_hints.end(); ++it) {
    _hints.push_back(*it);
  }

};

nlohmann::json  Hints::to_json() {

  nlohmann::json obj = Arguments::to_json();

  nlohmann::json& arguments = obj["arguments"];
  arguments["hints"] = _hints;

  obj["arguments"] = arguments;

  return obj;
};


Winner::Winner() {
  _name = "winner";
};

Winner::Winner(nlohmann::json& arguments) {
  _name = arguments["method"];
  _winner = arguments["arguments"]["winner"];
};

nlohmann::json  Winner::to_json() {

  nlohmann::json obj = Arguments::to_json();

  nlohmann::json& arguments = obj["arguments"];
  arguments["winner"] = _winner;

  obj["arguments"] = arguments;

  return obj;
};
