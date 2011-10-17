#include <nmpython.h>
#include <nmstring.h>

int main(int argc, char** argv)
{
	try // always check for Python_exceptions

	{
		const char * p1 = "mgen.py";
		const char * p2 = "3";
		const char * p3 = "entrada.dly";
		const char * p4 = "output.xff";
		char * args[4];
		args[0] = (char *)p1;
		args[1] = (char *)p2;
		args[2] = (char *)p3;
		args[3] = (char *)p4;

		NMPython py(4, args);
	
		// make the call
		py.runFile("mgen.py");
	
		std::cout << "FINISH!" << std::endl; 
	
		return 0;
	}
	catch (NMPythonException ex)
	{
		std::cout << ex.what();
	}
	return 0;
}
