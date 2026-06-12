#pragma once

# include <iostream>
# include <exception>
# include <fstream>

unsigned char *loadBMP(const std::string& fileName, int& height, int& width);
