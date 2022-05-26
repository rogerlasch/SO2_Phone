
/**
*Escrito por R�ger Matheus Lasch em Maio de 2022.
*Defini��o da classe Person.
*Esta biblioteca foi utilizada para desenvolver o trabalho de threads de SO2.
**/

#ifndef PERSON_H
#define PERSON_H


/**
*Define as flags que se aplicam a esta classe...
**/
enum Person_flags
{
ps_calling=1,//A Person est� ligando...
ps_buzy=2,//A pessoa est� ocupada...
ps_remove=4,//A Person j� fez sua liga��o, por tanto, n�o precisa mais competir pelo telefone.
};


/**
*A classe Person herda da classe BasicFlags por conveni�ncia para acessar os m�todos de manuseio de flags.
*A classe Person tamb�m � threadsafe e armazena apenas o atributo nome.
**/
class Person : public BasicFlags
{
private:
std::string name;
mutable std::shared_mutex mtx_person;
public:
//Construtor... O construtor de c�pia e de atribui��o s�o exclu�dos.
Person(const std::string& name="");
Person(const Person& ps)=delete;
Person& operator=(const Person& ps)=delete;
void setName(const std::string& name);
std::string getName()const;
//Verifica se a pessoa est� fazendo uma chamada telef�nica...
bool isCalling()const;
};

//Aqui, iremos criar um alias que iremos usar.
//shared_ptr � uma classe que encapsula um ponteiro e libera a mem�ria automaticamente quando a �ltima refer�ncia sair do escopo.
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
//Adquire permiss�o de escrita... Ningu�m mais pode acessar esta vari�vel enquanto a opera��o estiver em andamento.
std::unique_lock<std::shared_mutex> lck(this->mtx_person);
this->name=name;
}

std::string Person::getName()const
{
//Adquire permiss�o de leitura, ningu�m mais pode escrever enquanto um ou mais threads estiverem lendo esta vari�vel.
std::shared_lock lck(this->mtx_person);
return this->name;
}

bool Person::isCalling()const
{
//Verifica se a flag ps_calling est� definida. Caso esteja, significa que a pessoa est� fazendo uma chamada telef�nica.
return flag_contains(ps_calling);
}


std::ostream& operator<<(std::ostream& os, const shared_person& ps)
{
os<<ps->getName();
return os;
}
#endif
