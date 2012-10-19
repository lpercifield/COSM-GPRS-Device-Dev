#include <Cosm.h>

CosmFeed::CosmFeed(unsigned long aID, CosmDatastream* aDatastreams, int aDatastreamsCount, char mobileFeed, float flat, float flong)
  : _id(aID), _datastreams(aDatastreams), _datastreamsCount(aDatastreamsCount), _mobileFeed(mobileFeed), _flat(flat), _flong(flong)
{
  //strcpy(_id, aID);
}
void CosmFeed::setLoc(float flat, float flong)
{
	_flat = flat;
	_flong = flong;
}
size_t CosmFeed::printTo(Print& aPrint) const
{
  int len = 0;
  len += aPrint.println("{");
  len += aPrint.println("\"version\":\"1.0.0\",");
  if(_mobileFeed == 1 && _flat != 0.0 && _flong != 0.0){
  	len += aPrint.println("\"location\":{");
  	len += aPrint.println("\"disposition\":\"mobile\",");
  	len += aPrint.print("\"lat\":");
  	len += aPrint.print(_flat,5);
  	len += aPrint.println(",");
  	len += aPrint.print("\"lon\":");
  	len += aPrint.println(_flong,5);
  	len += aPrint.println("},");
  }
//   else{
//   	len += aPrint.println("\"location\":{");
//   	len += aPrint.println("\"disposition\":\"mobile\"");
//   	len += aPrint.println("},");
//   }
  len += aPrint.println("\"datastreams\" : [");
  for (int j =0; j < _datastreamsCount; j++)
  {
    len += aPrint.print(_datastreams[j]);
    if (j == _datastreamsCount-1)
    {
      // Last time through
      len += aPrint.println();
    }
    else
    {
      len += aPrint.println(",");
    }
  }
  len += aPrint.println("]");
  len += aPrint.println("}");
  return len;
}
// 
// "location":{
//       "disposition":"fixed",
//       "ele":"23.0",
//       "name":"office",
//       "lat":51.5235375648154,
//       "exposure":"indoor",
//       "lon":-0.0807666778564453,
//       "domain":"physical"
//   },

