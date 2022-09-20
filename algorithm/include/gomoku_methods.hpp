
#ifndef ARGUMENTS_HPP_
# define ARGUMENTS_HPP_

#include <nlohmann/json.hpp>

using json = nlohmann::json;


struct MethodArgs {
  using pointer = std::shared_ptr<MethodArgs>;

  virtual ~MethodArgs() {};
  virtual json as_json() = 0;
};


struct GomokuMethod
{
  using pointer = std::shared_ptr<GomokuMethod>;

  std::string           name;
  MethodArgs::pointer   arguments;

  json as_json();
};


struct EmptyArg : public MethodArgs {
  virtual ~EmptyArg() {};
  virtual json as_json();
};


struct StartGame : public MethodArgs
{
  virtual ~StartGame() {};
  virtual json as_json();

  std::string    mode;
  std::string    difficult;
  size_t         board_size;
  std::string    player_color;
};


struct Back : public MethodArgs
{
  virtual ~Back() {};
  virtual json as_json();

  std::string               color;
  std::string               position;
  std::vector<std::string>  captures;
};


struct MakeTurn : public MethodArgs
{
  virtual ~MakeTurn() {};
  virtual json as_json();

  std::string               color;
  std::string               position;
  std::vector<std::string>  captures;
  std::vector<std::string>  hints;
};


struct Hints : public MethodArgs
{
  virtual ~Hints() {};
  virtual json as_json();

  std::vector<std::string>  hints;
};


struct Winner : public MethodArgs
{
  virtual ~Winner() {};
  virtual json as_json();

  std::string  winner;
};

void to_json(json& j, const MakeTurn& p);
void from_json(const json& j, MakeTurn& p);

void to_json(json& j, const Winner& p);
void from_json(const json& j, Winner& p);

void to_json(json& j, const Hints& p);
void from_json(const json& j, Hints& p);

void to_json(json& j, const Back& p);
void from_json(const json& j, Back& p);

void to_json(json& j, const StartGame& p);
void from_json(const json& j, StartGame& p);

class ArgumentFactory {

public:

  static GomokuMethod::pointer createArguments(std::string method, json& arguments) {

    MethodArgs::pointer arg_ptr;

    if (method == "make_turn")
      arg_ptr = std::make_shared<MakeTurn>(arguments.get<MakeTurn>());
    else if (method == "print_hints")
      arg_ptr = std::make_shared<Hints>(arguments.get<Hints>());
    else if (method == "back")
      arg_ptr = std::make_shared<Back>(arguments.get<Back>());
    else if (method == "winner")
      arg_ptr = std::make_shared<Winner>(arguments.get<Winner>());
    else if (method == "start_game")
      arg_ptr = std::make_shared<StartGame>(arguments.get<StartGame>());
    else
      arg_ptr = MethodArgs::pointer(new EmptyArg());

    GomokuMethod gm = { method, arg_ptr };

    return std::make_shared<GomokuMethod>(gm);
  }
};


#endif // ARGUMENTS_HPP_
