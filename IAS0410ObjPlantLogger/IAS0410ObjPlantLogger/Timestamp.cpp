#include "Timestamp.h"

#pragma warning(disable:4996)

Timestamp::Timestamp()
{
	curr_time = time(NULL);
	tm_local = localtime(&curr_time);
}

std::string Timestamp::getLocalTime()
{
	curr_time = time(NULL);
	tm_local = localtime(&curr_time);
	
	strftime(timebuffer, 30, "%F %X\n", tm_local);
	std::string DateAndTime(timebuffer);

	return DateAndTime;
}

time_t Timestamp::curr_time = NULL;
tm* Timestamp::tm_local = nullptr;
char Timestamp::timebuffer[30] = { 0 };