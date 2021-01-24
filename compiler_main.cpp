#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char *argv[])
{
  std::string flag = argv[0];
  std::string sourcefile = argv[1];
  std::string destfile = argv[3];

  std::ifstream cfile(sourcefile);
  std::ofstream outfile (destfile); //create instance of file which we will print output to
  if (flag == "--translate"){
    //translator selection from cmd line




  }
  else if (flag == "-S"){
    //compiler selection from cmd line

  }


  outfile.close();
}
