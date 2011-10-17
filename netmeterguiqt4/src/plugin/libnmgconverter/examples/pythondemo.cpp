#include <nmpython.h>
#include <nmstring.h>

int main(int argc, char** argv)
{
  try // always check for Python_exceptions

    {

      // Create an instance of the interpreter.

      NMPython py(argc, argv);

      // Load the test module into the interpreter

      py.loadModule("mgen");

      NMPyArgMap args; 

      // fill up argument strings and their types.

      args["3"] = nmpyLong;
      args["entrada.dly"] = nmpyString;
      //args["sortida.xff"] = nmpyString;

      // make the call
      py.call("convert", args);

      // will print 200

      std::cout << "FINISH!" << std::endl; 
      args.clear();

      return 0;
    }
  catch (NMPythonException ex)
    {
      std::cout << ex.what() << std::endl;
    }
  return 0;
}
