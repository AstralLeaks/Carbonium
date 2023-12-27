#pragma once

#include "strencrypt.hpp"
const char* ProxyHost = ENC("");

#define PROD
//#define SENDLOGSTOCONSOLE
#define WRITELOGSTOFILE
//#define READFILE

//add a devname variable to the dll
const char* Backend = "smth";
const char* eos = "api.epicgames.dev";
constexpr bool INGAME = true;

#ifdef READFILE // man that 1kb is important ok :/
std::string myText;
std::string hybridyesorno;
std::ifstream MyReadFile("info.txt");
#endif

#ifndef READFILE
constexpr bool bIsHybrid = false;
#endif
#ifdef READFILE
bool bIsHybrid;
#endif
