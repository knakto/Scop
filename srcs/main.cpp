#include <exception>
#include <iostream>
#include "Application.hpp"

int main(int c, char **v)
{
  if (c != 2)
  {
    std::cerr << "usage: ./scop <filename>.obj" << std::endl;
    return 1;
  }
  Application *app = nullptr;
  try{
    app = new Application("scop", 1920, 1080);
    app->bindObjectByName(v[1]);
    app->run();
    delete app;
  }
  catch ( const std::exception& e ){
    if (app != nullptr)
      delete app;
    std::cerr << e.what() << std::endl;
  }
}
