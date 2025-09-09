#include <CliCore/source/Entry.h>
#include <cstdio>

int main(int argc, char** argv)
{
	try {
		return p2c::cli::Entry(argc, argv);
	}
	catch (...) {
		puts("\nError: Unhandled exception.\n");
		return -1;
	}
}