#include "AGTime.h"

using namespace std;

AGTime::AGTime(const AGTime& time)
{
	sec = time.sec;
}

int
AGTime::convert(int days, int hours, int minutes, int seconds)
{
	sec = seconds +60*( minutes + 60*( hours + 24*(days) ) );
}

int
AGTime::getSecondsOf(float minutes)
{
	float seconds = 60.0 * minutes;
	return (int) seconds;
}

bool
AGTime::operator==(const AGTime &time)
{
	if(this->sec == time.sec)
		return true;
	else
		false;
}

bool
AGTime::operator<(const AGTime &time)
{
	if(this->sec < time.sec)
		return true;
	else
		false;
}

bool
AGTime::operator<=(const AGTime &time)
{
	if(this->sec <= time.sec)
		return true;
	else
		false;
}

void
AGTime::operator+=(const AGTime &time)
{
	this->sec += time.sec;
}

void
AGTime::operator-=(const AGTime &time)
{
	this->sec -= time.sec;
}

AGTime
AGTime::operator+(const AGTime &time)
{
	AGTime newtime(time.sec + this->sec);
	return newtime;
}

int
AGTime::getDay()
{
	return (sec / 86400);
}

int
AGTime::getHour()
{
	return ((sec / 3600) % 24);
}

int
AGTime::getMinute()
{
	return ((sec / 60) % 60);
}

int
AGTime::getSecond()
{
	return (sec % 60);
}

int
AGTime::getTime()
{
	return this->sec;
}

void
AGTime::setDay(int d)
{
	if(0 <= d)
	{
		sec -= 86400 * getDay();
		sec += 86400 * d;
	}
}

void
AGTime::setHour(int h)
{
	if(0 <= h && h < 24)
	{
		sec -= 3600 * getHour();
		sec += 3600 * h;
	}
}

void
AGTime::setMinute(int m)
{
	if(0 <= m && m < 60)
	{
		sec -= 60 * getMinute();
		sec += 60 * m;
	}
}

void
AGTime::setSecond(int s)
{
	if(0 <= s && s < 60)
	{
		sec -= getSecond();
		sec += s;
	}
}

void
AGTime::setTime(int sec)
{
	this->sec = sec;
}

void
AGTime::addDays(int d)
{
	sec += 86400 * d;
}

void
AGTime::addHours(int h)
{
	sec += 3600 * h;
}

void
AGTime::addMinutes(int m)
{
	sec += 60 * m;
}

void
AGTime::addSeconds(int s)
{
	sec += s;
}
