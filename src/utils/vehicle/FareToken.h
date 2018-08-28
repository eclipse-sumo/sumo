//
// Created by bzfeuler on 16.08.18.
//

#ifndef SUMO_FARETOKEN_H
#define SUMO_FARETOKEN_H
enum class FareToken : int
{
  None =  0,
  Free =  1, // walking and other things can be free
  H    =  2,
  L    =  3,
  T1   =  4,
  T2   =  5,
  T3   =  6,
  Z    =  7,
  M    =  8,
  U    =  9,
  KL   =  10,
  KH   =  11,
  K    =  12,
  KHU  =  13,
  KLU  =  14,
  KHZ  =  15,
  KLZ  =  16,
  ZU   =  17   //Artificial state for after leaving short trip khu or klu since a pedestrian edge does not allow us
              //to distinguish between z or u zones
};

namespace FareUtil {
  
  inline std::string tokenToString(FareToken const &token)
  {
    switch (token)
    {
      case FareToken::H:
        return "H";
      case FareToken::L:
        return "L";
      case FareToken::T1:
        return "T1";
      case FareToken::T2:
        return "T2";
      case FareToken::T3:
        return "T3";
      case FareToken::U:
        return "U";
      case FareToken::Z:
        return "Z";
      case FareToken::M:
        return "M";
      case FareToken::K:
        return "K";
      case FareToken::KL:
        return "KL";
      case FareToken::KH:
        return "KH";
      case FareToken::ZU:
        return "ZU";
      case FareToken::None:
        return "None";
      case FareToken::Free:
        return "Free";
      case FareToken::KHU:
        return "KHU";
      case FareToken::KLU:
        return "KLU";
      case FareToken ::KHZ:
        return "KHZ";
      case FareToken ::KLZ:
        return  "KLZ";
    }
    return ""; //surpress compiler warning
  }
  
  inline FareToken stringToToken(std::string str)
  {
    if (str == "H") return FareToken::H;
    if (str == "L") return FareToken::L;
    if (str == "T1") return FareToken::T1;
    if (str == "T2") return FareToken::T2;
    if (str == "T3") return FareToken::T3;
    if (str == "1") return FareToken::T1;
    if (str == "2") return FareToken::T2;
    if (str == "3") return FareToken::T3;
    if (str == "U") return FareToken::U;
    if (str == "Z") return FareToken::Z;
    if (str == "M") return FareToken::M;
    if (str == "K") return FareToken::K;
    if (str == "KL") return FareToken::KL;
    if (str == "KH") return FareToken::KH;
    if (str == "ZU") return FareToken::ZU;
    if (str == "None") return FareToken::None;
    if (str == "Free") return FareToken::Free;
    if (str == "KHU") return FareToken::KHU;
    if (str == "KLU") return FareToken::KLU;
    if (str == "KHZ") return FareToken::KHZ;
    if (str == "KLZ") return FareToken::KLZ;
    if (str == "NOTFOUND") return FareToken::None;
    assert(false);
    return FareToken::None;
  }
  
}
#endif //SUMO_FARETOKEN_H
