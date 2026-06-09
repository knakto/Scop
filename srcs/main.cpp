#include <exception>
#include <iostream>
#include "Application.hpp"
#include "ObjectData.hpp"

int main(int c, char **v)
{
  if (c != 2)
  {
    std::cerr << "usage: ./scop <filename>.obj" << std::endl;
    return 1;
  }
  try{
    ObjectData *obj = new ObjectData(v[1]);
    Application *app = new Application("scop", 1920, 1080);

    app->run(obj);
    delete app;
    delete obj;
  }
  catch ( const std::exception& e ){
      std::cout << e.what() << std::endl;
  }
}
