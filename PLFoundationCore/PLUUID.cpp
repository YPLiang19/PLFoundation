//
//  PLUUID.cpp
//  PLFoundation
//
//  Created by yongpengliang on 2020/8/20.
//  Copyright © 2020 yongpengliang. All rights reserved.
//

#include "PLUUID.h"

using namespace std;

PLFOUNDATON_NAMESPACE_BEGIN

static const int kUUIDStringLength = 36;
static const int kUnformattedUUIDStringLength = 32;
static const int kUUIDByteCount = 16;


static int uuid_from_string(const char *string, unsigned char *uuid)
{
  char    unformatted[kUnformattedUUIDStringLength];
  int    i;

  if (strlen(string) != kUUIDStringLength)
    {
      return -1;
    }
  for (i = 0; i < kUUIDStringLength; i++)
    {
      char c = string[i];

      if ((i == 8) || (i == 13) || (i == 18) || (i == 23))
    {
      if (c != '-')
        {
          return -1;
        }
    }
      else
    {
      if (!isxdigit(c))
        {
          return -1;
        }
    }
    }
  strncpy(unformatted, string, 8);
  strncpy(unformatted+8, string+9, 4);
  strncpy(unformatted+12, string+14, 4);
  strncpy(unformatted+16, string+19, 4);
  strncpy(unformatted+20, string+24, 12);

  for (i = 0; i < kUUIDByteCount; i++)
    {
      {
    char thisDigit[3];
    thisDigit[0] = unformatted[2*i];
    thisDigit[1] = unformatted[2*i+1];
    thisDigit[2] = 0;
    uuid[i] = strtoul(thisDigit, NULL, kUUIDByteCount);
      }
    }
  return 0;
}

static void string_from_uuid(const unsigned char *uuid, char *string)
{
  char    unformatted[kUnformattedUUIDStringLength];
  int    i;

  for (i = 0; i < kUUIDByteCount; i++)
    {
      unsigned char byte = uuid[i];
      char thisPair[3];
      snprintf(thisPair, 3, "%02X", byte);
      strncpy(unformatted + 2*i, thisPair, 2);
    }
  strncpy(string, unformatted, 8);
  string[8] = '-';
  strncpy(string + 9, unformatted + 8, 4);
  string[13] = '-';
  strncpy(string + 14, unformatted + 12, 4);
  string[18] = '-';
  strncpy(string + 19, unformatted + 16, 4);
  string[23] = '-';
  strncpy(string + 24, unformatted + 20, 12);
  string[kUUIDStringLength] = '\0';
}


static int random_uuid(unsigned char *uuid)
{
  unsigned char timeByte;
  unsigned char sequenceByte;

  /* Only supporting Version 4 UUIDs (see RFC4412, section 4.4),
   * consistent with Apple.  Other variants suffer from privacy
   * problems (and are more work...)
   */
    
    for (int i = 0; i < kUUIDByteCount;  i++) {
        uuid[i] = rand() % 256;
    }

  /* as required by the RFC, bits 48-51 should contain 0b0100 (4)
   * and bits 64-65 should contain 0b01 (1)
   */
  timeByte = uuid[6];
  timeByte = (4 << 8) + (timeByte & 0x0f);
  uuid[7] = timeByte;

  sequenceByte = uuid[8];
  sequenceByte = (1 << 6) + (sequenceByte & 0x3f);
  uuid[8] = sequenceByte;

  return 0;
}


PLUUID PLUUID::UUID(){
    uuid_t localUUID;
    random_uuid(localUUID);
    return PLUUID(localUUID);
    
}

PLUUID::PLUUID(std::string str){
    uuid_t localUUID;
    uuid_from_string(str.c_str(), localUUID);
    memcpy(_uuid, localUUID, kUUIDByteCount);
    
}

PLUUID::PLUUID(uuid_t bytes){
    memcpy(_uuid, bytes, kUUIDByteCount);
}

std::string PLUUID::UUIDString(){
    char uuidChars[kUUIDStringLength + 1];
    string_from_uuid(_uuid, uuidChars);
    std::string str(uuidChars);
    return str;
}

void PLUUID::getUUIDBytes(uuid_t btyes){
    memcpy(btyes, _uuid, kUUIDByteCount);
}

PLFOUNDATON_NAMESPACE_END
