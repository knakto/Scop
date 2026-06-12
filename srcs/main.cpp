#include <exception>
#include <iostream>
// #include "ObjectData.hpp"
#include "Application.hpp"
#include "Object.hpp"
#include "Image.hpp"

int main(int c, char **v)
{
  if (c != 3)
  {
    std::cerr << "usage: ./scop <filename>.obj <texturefile>.bmp" << std::endl;
    return 1;
  }
  try{
    Object obj(v[1], v[2]);
    Application *app = new Application("scop", 1920, 1080);

    app->run(obj);
    delete app;
  }
  catch ( const std::exception& e ){
      std::cout << e.what() << std::endl;
  }
}
