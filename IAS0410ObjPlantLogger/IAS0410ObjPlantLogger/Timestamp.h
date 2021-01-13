#pragma once
#include <ctime>
#include <string>


class Timestamp
{
private:
	static time_t curr_time;
	static tm* tm_local;
	static char timebuffer[30];
public:
	Timestamp();

	static std::string getLocalTime();

};

