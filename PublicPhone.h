


/**
*Escrito por Róger Matheus Lasch em Maio de 2022.
*Classe que representa um telefone público.
*Esta biblioteca foi utilizada para desenvolver o trabalho de threads de SO2.
**/

#ifndef PUBLICPHONE_H
#define PUBLICPHONE_H

//Flags que se aplicam a esta classe...
enum PublicPhone_flags
{
phone_calling=1,//Significa que alguém está ligando...
phone_buzy=2,//O telefone está ocupado por alguém...
};

/**
*O telefone só permite que uma pessoa por vês possa fazer uma ligação.
*Ele executa os seguintes passos:
*Vários threads acessam o método enter.
*O método entra em loop até que consegue o direito exclusivo para fazer uma ligação.
*Depois da ligação feita, o telefone deve ficar livre para outra pessoa.
*A classe herda de BasicFlags por conveniência para acessar os métodos de manuseio de flags.
**/
class PublicPhone : public BasicFlags
{
public:
//A pessoa que atualmente está telefonando...
shared_person person;
//Controla o acesso ao telefone...
mutable std::shared_mutex mtx_phone;
public:
PublicPhone();
PublicPhone(const PublicPhone& ps)=delete;
PublicPhone& operator=(const PublicPhone& ps)=delete;
bool isCalling()const;
bool isBuzy()const;
//Entra e fica competindo pelo telefone até conseguir fazer aligação...
void enter(shared_person& ps);
//Simula um telefonema...
void make_call();
};

//shared_ptr é uma classe que encapsula um ponteiro e libera a memória automaticamente quando a última referência sair do escopo.
typedef std::shared_ptr<PublicPhone> shared_publicphone;

#endif


#ifndef PUBLICPHONE_IMPLEMENTATION
#define PUBLICPHONE_IMPLEMENTATION

PublicPhone::PublicPhone() : BasicFlags()
{
person=NULL;
}

//Verifica se o telefone está fazendo uma ligação...
bool PublicPhone::isCalling()const
{
return flag_contains(phone_calling);
}

//Verifica se o telefone está ocupado...
bool PublicPhone::isBuzy()const
{
return flag_contains(phone_buzy);
}

/**
*Primeiro,entra em loop até que a pessoa tenha feito uma ligação.
*Depois, tenta adiquirir direito sobre o telefone.
*Caso consiga o acesso ao telefone, faz a ligação e libere quando terminar.
**/
void PublicPhone::enter(shared_person& ps)
{
while(!ps->flag_contains(ps_remove))
{
//Inicializa uma trava exclusiva, mas não tenta adiquirir. O defer_lock define que apenas controla o objeto.
//O bloqueio é automaticamente livre a sim que o objeto sair do escopo e for destruído.
std::unique_lock<std::shared_mutex> lck(this->mtx_phone, std::defer_lock);
//3 condições precisam ser atendidas para que uma ligação seja feita.
//Primeiro, lck precisa conseguir o acesso exclusivo. try_lock deve retornar true.
//segundo: O telefone não pode estar ocupado. flag_contains(phone_buzy)==false.
//Terceiro: A pessoa não pode ter feito uma ligação anteriormente. ps->flag_contains(ps_remove)==false
//Caso lck.try_lock retorne true, ninguém mais conseguirá o acesso exclusivo até que o bloqueio seja desfeito com unlock, ou lck seja destruído.
if((lck.try_lock())&&(!this->flag_contains(phone_buzy))&&(!ps->flag_contains(ps_remove)))
{
_log("{} vai falar", ps->getName());
this->setflag(phone_buzy|phone_calling);
ps->setflag(ps_buzy|ps_calling);
this->person=ps;
//Faça a ligação...
make_call();
this->person=shared_person();
ps->removeflag(ps_calling|ps_buzy);
ps->setflag(ps_remove);
this->removeflag(phone_buzy|phone_calling);
//break;
}
else//Uma ou mais das condições a cima não foram atendidas...
{
_log("{} está aguardando.", ps->getName());
//Espere um tempo antes de tentar fazer a ligação de novo...
waitms(random_int32(3000, 6000));
}
}
}

//Simula uma ligação telefônica...
void PublicPhone::make_call()
{
int32 x=random_int32(5000, 12000);
_log("{} está falando no telefone...", person->getName());
waitms(x);
_log("{} desligou o telefone e saiu.", person->getName());
}
#endif
