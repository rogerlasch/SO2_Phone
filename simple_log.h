


/**
*Escrito por Róger Matheus Lasch em Maio de 2022.
*Pequena biblioteca para output.
*Esta biblioteca foi utilizada para desenvolver o trabalho de threads de SO2.
**/
#ifndef SIMPLE_LOG_H
#define SIMPLE_LOG_H


#include<format>//Apenas c++20 em diante...
#include<iostream>
#include<fstream>

void simple_log(const std::string& s)
{
static std::ofstream ofn("log.txt");
ofn<<s<<std::endl;
std::cout<<s<<std::endl;
}

//Define macro para formatar uma string e logar/printar.
#define _log(str, ...) simple_log(std::format(str, __VA_ARGS__))
#endif
