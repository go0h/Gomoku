
#ifndef ARGUMENTS_HPP_
# define ARGUMENTS_HPP_

#include <nlohmann/json.hpp>

using json = nlohmann::json;


class Arguments : public std::enable_shared_from_this<Arguments>
{

public:

  using pointer = std::shared_ptr<Arguments>;

  virtual ~Arguments() = default;

  virtual nlohmann::json  to_json();
  std::string             to_json_string();

protected:

  std::string _name;
};


class ArgumentFactory {

private:

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
    ~MakeTurn() {};

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

    nlohmann::json  to_json();

  private:

    std::string  _winner;
  };

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
