
#ifndef COSM_FEED_H
#define COSM_FEED_H

#include <Client.h>
#include <CosmDatastream.h>
#include <Printable.h>

class CosmFeed : public Printable
{
public:
  CosmFeed(unsigned long aID, CosmDatastream* aDatastreams, int aDatastreamsCount, char mobileFeed = 0, float flat = 0.0, float flong = 0.0);
  void setLoc( float flat, float flong);
  virtual size_t printTo(Print&) const;
  unsigned long id() { return _id; };
  int size() { return _datastreamsCount; };
  //char isMobile() { return _mobileFeed; };
  CosmDatastream& operator[] (unsigned i) { return _datastreams[i]; };
protected:
  unsigned long _id;
  char _mobileFeed;
  float _flat;
  float _flong;
  CosmDatastream* _datastreams;
  int _datastreamsCount;
};

#endif

