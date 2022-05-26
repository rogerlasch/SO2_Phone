
/**
*Escrito por Róger Matheus Lasch em Maio de 2022.
*Arquivo principal que poem tudo em funcionamento.
*Compilado com o visual studio comunity 2022 e com c++ 20..
*Para compilar, abra um prompt de comandos do desenvolvedor e navegue com cd até a pasta em que este arquivo se encontra.
*Depois rode o comando:
*cl /nologo /EHsc /std:c++latest *.cpp
*O programa simula um telefone público com várias pessoas tentando fazer uma ligação.
*O telefone precisa gerenciar o acesso das pessoas. Só mente uma pessoa por vez pode fazer uma ligação.
*Quando uma pessoa conseguir o acesso ao telefone, as outras devem esperar e tentar novamente mais tarde.
*Quando a ligação terminar, a pessoa deve ceder o telefone para que outra pessoa consiga utilizar.
**/
#include<fstream>
#include<thread>//Funções assíncronas
#include<chrono>//Biblioteca de tempo...
#include<functional>//std::ref
#include<mutex>//Unique_lock...
#include<shared_mutex>//Bloqueio compartilhado... e shared_lock...
#include<memory>//shared_ptr e make_shared...

//Macro para por o thread atual dormir por x millisegundos.
#define waitms(ms) std::this_thread::sleep_for(std::chrono::milliseconds(ms))

#include"ed3_lib.h"
#include"simple_log.h"
#include"BasicFlags.h"
#include"Person.h"
#include"PublicPhone.h"

using namespace std;

//Protótipo de funções...
void person_load(vector<shared_person>&  persons);
void interact_phone(shared_person& ps, shared_publicphone& phone);

int main()
{
setlocale(LC_ALL, "Portuguese");
_log("Simulador de telefone público para demonstrar o acesso controlado a recursos...");
//Array de pessoas...
vector<shared_person> persons;
//Vetor de threads...
vector<thread> threads;
person_load(persons);
//Instancie um telefone público...
shared_publicphone phone=make_shared<PublicPhone>();
//Percorra a lista de pessoas e coloque cada uma em um thread para fazer a ligação...
for(auto it=persons.begin(); it!=persons.end(); ++it)
{
    threads.emplace_back(interact_phone, std::ref((*it)), std::ref(phone));
}
//Percorre o vetor de threads e espere todos terminarem as ligações antes de deixar o thread principal sair.
for(auto& th : threads)
{
th.join();
}
_log("Programa finalizado.");
return 0;
}

//Inicializa algumas pessoas...
void person_load(vector<shared_person>&  persons)
{
    vector<string> names={"Ana", "Paula", "Rodrigo", "Amanda", "Gabriel"};
    persons.clear();
    for(auto it=names.begin(); it!=names.end(); ++it)
    {
        shared_person ps=make_shared<Person>((*it));
        persons.push_back(ps);
    }
}

//A função que será chamada assincronamente para fazer a pessoa interagir com o telefone público...
void interact_phone(shared_person& ps, shared_publicphone& phone)
{
phone->enter(ps);
}
