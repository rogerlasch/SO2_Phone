
/**
*Escrito por Róger Matheus Lasch em Maio de 2022.
*Definição da classe Person.
*Esta biblioteca foi utilizada para desenvolver o trabalho de threads de SO2.
**/

#ifndef PERSON_H
#define PERSON_H


/**
*Define as flags que se aplicam a esta classe...
**/
enum Person_flags
{
ps_calling=1,//A Person está ligando...
ps_buzy=2,//A pessoa está ocupada...
ps_remove=4,//A Person já fez sua ligação, por tanto, não precisa mais competir pelo telefone.
};


/**
*A classe Person herda da classe BasicFlags por conveniência para acessar os métodos de manuseio de flags.
*A classe Person também é threadsafe e armazena apenas o atributo nome.
**/
class Person : public BasicFlags
{
private:
std::string name;
mutable std::shared_mutex mtx_person;
public:
//Construtor... O construtor de cópia e de atribuição são excluídos.
Person(const std::string& name="");
Person(const Person& ps)=delete;
Person& operator=(const Person& ps)=delete;
void setName(const std::string& name);
std::string getName()const;
//Verifica se a pessoa está fazendo uma chamada telefônica...
bool isCalling()const;
};

//Aqui, iremos criar um alias que iremos usar.
//shared_ptr é uma classe que encapsula um ponteiro e libera a memória automaticamente quando a última referência sair do escopo.
typedef std::shared_ptr<Person> shared_person;


std::ostream& operator<<(std::ostream& os, const shared_person& p);

#endif

#ifndef PERSON_IMPLEMENTATION_H
#define PERSON_IMPLEMENTATION_H
Person::Person(const std::string& name) : BasicFlags()
{
this->setName(name);
}

void Person::setName(const std::string& name)
{
//Adquire permissão de escrita... Ninguém mais pode acessar esta variável enquanto a operação estiver em andamento.
std::unique_lock<std::shared_mutex> lck(this->mtx_person);
this->name=name;
}

std::string Person::getName()const
{
//Adquire permissão de leitura, ninguém mais pode escrever enquanto um ou mais threads estiverem lendo esta variável.
std::shared_lock lck(this->mtx_person);
return this->name;
}

bool Person::isCalling()const
{
//Verifica se a flag ps_calling está definida. Caso esteja, significa que a pessoa está fazendo uma chamada telefônica.
return flag_contains(ps_calling);
}


std::ostream& operator<<(std::ostream& os, const shared_person& ps)
{
os<<ps->getName();
return os;
}
#endif
