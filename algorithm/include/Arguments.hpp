
#ifndef ARGUMENTS_HPP_
# define ARGUMENTS_HPP_

#include <nlohmann/json.hpp>

using json = nlohmann::json;


class Arguments : public std::enable_shared_from_this<Arguments>
{

public:

  using pointer = std::shared_ptr<Arguments>;

  virtual ~Arguments() = default;

  virtual std::string&    getName();
  virtual nlohmann::json  to_json();
  std::string             to_json_string();

protected:

  std::string _name;
};


class EmptyArg : public Arguments
{

public:

  EmptyArg() {};
  ~EmptyArg() {};

  nlohmann::json  to_json() { return json::object(); };
  std::string     to_json_string() { return ""; };
};


class EndGame : public Arguments
{

public:

  EndGame();
  EndGame(nlohmann::json& arguments);
  ~EndGame() {};
};


class Back : public Arguments
{

public:

  Back();
  Back(nlohmann::json& arguments);
  ~Back() {};

  std::string& getColor() { return _color; };
  std::string& getPosition() { return _position; };
  std::vector<std::string>& getCaptured() { return _captured; };

  nlohmann::json  to_json();

private:

  std::string               _color;
  std::string               _position;
  std::vector<std::string>  _captured;
};


class MakeTurn : public Arguments
{

public:

  MakeTurn();
  MakeTurn(nlohmann::json& arguments);
  MakeTurn(std::string color,
           std::string position,
           std::vector<std::string> captured,
           std::vector<std::string> hints);
  ~MakeTurn() {};

  std::string& getColor() { return _color; };
  std::string& getPosition() { return _position; };
  std::vector<std::string>& getCaptured() { return _captured; };
  std::vector<std::string>& getHints() { return _hints; };

  nlohmann::json  to_json();

private:

  std::string               _color;
  std::string               _position;
  std::vector<std::string>  _captured;
  std::vector<std::string>  _hints;
};


class Hints : public Arguments
{

public:

  Hints();
  Hints(nlohmann::json& arguments);
  ~Hints() {};

  std::vector<std::string>& getHints() { return _hints; };
  nlohmann::json  to_json();

private:

  std::vector<std::string>  _hints;
};


class Winner : public Arguments
{

public:

  Winner();
  Winner(nlohmann::json& arguments);
  ~Winner() {};

  std::string& getWinner() { return _winner; };
  nlohmann::json  to_json();

private:

  std::string  _winner;
};


class ArgumentFactory {

public:

  static Arguments* createArguments(nlohmann::json& arguments) {

    std::string method = arguments["method"];

    if (method == "make_turn")
      return new MakeTurn(arguments);
    else if (method == "winner")
      return new Winner(arguments);
    else if (method == "print_hints")
      return new Hints(arguments);
    else if (method == "back")
      return new Back(arguments);

    return new EndGame(arguments);
  }

  static Arguments::pointer createArgPtr(nlohmann::json& arguments) {
    return Arguments::pointer(createArguments(arguments));
  }

};

#endif // ARGUMENTS_HPP_
