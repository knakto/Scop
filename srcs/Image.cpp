#include "Image.hpp"
#include <exception>
#include <stdexcept>

unsigned char *loadBMP(const std::string& fileName, int& height, int& width)
{
  std::fstream file;
  try{ 
    if (fileName.substr(fileName.rfind('.')) != ".bmp")
      throw std::runtime_error("file image is not .bmp");

    char header[54];

    file.open(fileName, std::ios_base::in | std::ios_base::binary);
    if (!file.is_open())
      throw std::runtime_error("file image .bmp not found");

    file.read(header, 54);
    if (!(header[0] == 'B' && header[1] == 'M'))
      throw std::runtime_error("Invalid format file.");

    width  = *(int*)&(header[18]);
    height = *(int*)&(header[22]);
    int imageSize  = *(int*)&(header[34]);

    if (imageSize == 0)
      imageSize = width * height * 3;

    char *data = new char[imageSize];
    file.read(data, imageSize);

    for (int i = 0; i < imageSize; i += 3)
    {
      char swap = data[i];
      data[i] = data[i + 2];
      data[i + 2] = swap;
    }

    file.close();

    return (unsigned char*)data;
  } catch (const std::runtime_error& e)
  {
    file.close();
    throw e;
  }
}
