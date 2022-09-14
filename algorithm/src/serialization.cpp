
#include "gomoku_methods.hpp"


nlohmann::json GomokuMethod::as_json() {
  json js = json::object();
  js["method"] = name;
  js["arguments"] = arguments->as_json();
  return js;
}


nlohmann::json MakeTurn::as_json() {
  json js = json::object();
  to_json(js, *this);
  return js;
}


nlohmann::json Back::as_json() {
  json js = json::object();
  to_json(js, *this);
  return js;
}


nlohmann::json Hints::as_json() {
  json js = json::object();
  to_json(js, *this);
  return js;
}


nlohmann::json Winner::as_json() {
  json js = json::object();
  to_json(js, *this);
  return js;
}


nlohmann::json EmptyArg::as_json() {
  return json::object();
}


nlohmann::json StartGame::as_json() {
  json js = json::object();
  to_json(js, *this);
  return js;
}


void to_json(json& j, const MakeTurn& p) {
  j = json {
    {"color",    p.color},
    {"position", p.position},
    {"captures", p.captures},
    {"hints",    p.hints}
  };
}


void from_json(const json& j, MakeTurn& p) {
  j.at("color").get_to(p.color);
  j.at("position").get_to(p.position);
  j.at("captures").get_to(p.captures);
  if (j.find("hints") != j.end())
    j.at("hints").get_to(p.hints);
}


void to_json(json& j, const Back& p) {
  j = json {
    {"color",    p.color},
    {"position", p.position},
    {"captures", p.captures}
  };
}


void from_json(const json& j, Back& p) {
  j.at("color").get_to(p.color);
  j.at("position").get_to(p.position);
  j.at("captures").get_to(p.captures);
}


void to_json(json& j, const Hints& p) {
  j = json {
    {"hints",    p.hints}
  };
}


void from_json(const json& j, Hints& p) {
  j.at("hints").get_to(p.hints);
}


void to_json(json& j, const Winner& p) {
  j = json {
    {"winner",    p.winner}
  };
}


void from_json(const json& j, Winner& p) {
  j.at("winner").get_to(p.winner);
}


void to_json(json& j, const StartGame& p) {
  j = json {
    {"mode",          p.mode},
    {"difficult",     p.difficult},
    {"board_size",    p.board_size},
    {"player_color",  p.player_color}
  };
}


void from_json(const json& j, StartGame& p) {
  j.at("mode").get_to(p.mode);
  j.at("difficult").get_to(p.difficult);
  j.at("board_size").get_to(p.board_size);
  j.at("player_color").get_to(p.player_color);
}
