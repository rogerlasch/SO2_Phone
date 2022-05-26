


/**
*Escrito por R�ger Matheus Lasch em Maio de 2022.
*Classe que representa um telefone p�blico.
*Esta biblioteca foi utilizada para desenvolver o trabalho de threads de SO2.
**/

#ifndef PUBLICPHONE_H
#define PUBLICPHONE_H

//Flags que se aplicam a esta classe...
enum PublicPhone_flags
{
phone_calling=1,//Significa que algu�m est� ligando...
phone_buzy=2,//O telefone est� ocupado por algu�m...
};

/**
*O telefone s� permite que uma pessoa por v�s possa fazer uma liga��o.
*Ele executa os seguintes passos:
*V�rios threads acessam o m�todo enter.
*O m�todo entra em loop at� que consegue o direito exclusivo para fazer uma liga��o.
*Depois da liga��o feita, o telefone deve ficar livre para outra pessoa.
*A classe herda de BasicFlags por conveni�ncia para acessar os m�todos de manuseio de flags.
**/
class PublicPhone : public BasicFlags
{
public:
//A pessoa que atualmente est� telefonando...
shared_person person;
//Controla o acesso ao telefone...
mutable std::shared_mutex mtx_phone;
public:
PublicPhone();
PublicPhone(const PublicPhone& ps)=delete;
PublicPhone& operator=(const PublicPhone& ps)=delete;
bool isCalling()const;
bool isBuzy()const;
//Entra e fica competindo pelo telefone at� conseguir fazer aliga��o...
void enter(shared_person& ps);
//Simula um telefonema...
void make_call();
};

//shared_ptr � uma classe que encapsula um ponteiro e libera a mem�ria automaticamente quando a �ltima refer�ncia sair do escopo.
typedef std::shared_ptr<PublicPhone> shared_publicphone;

#endif


#ifndef PUBLICPHONE_IMPLEMENTATION
#define PUBLICPHONE_IMPLEMENTATION

PublicPhone::PublicPhone() : BasicFlags()
{
person=NULL;
}

//Verifica se o telefone est� fazendo uma liga��o...
bool PublicPhone::isCalling()const
{
return flag_contains(phone_calling);
}

//Verifica se o telefone est� ocupado...
bool PublicPhone::isBuzy()const
{
return flag_contains(phone_buzy);
}

/**
*Primeiro,entra em loop at� que a pessoa tenha feito uma liga��o.
*Depois, tenta adiquirir direito sobre o telefone.
*Caso consiga o acesso ao telefone, faz a liga��o e libere quando terminar.
**/
void PublicPhone::enter(shared_person& ps)
{
while(!ps->flag_contains(ps_remove))
{
//Inicializa uma trava exclusiva, mas n�o tenta adiquirir. O defer_lock define que apenas controla o objeto.
//O bloqueio � automaticamente livre a sim que o objeto sair do escopo e for destru�do.
std::unique_lock<std::shared_mutex> lck(this->mtx_phone, std::defer_lock);
//3 condi��es precisam ser atendidas para que uma liga��o seja feita.
//Primeiro, lck precisa conseguir o acesso exclusivo. try_lock deve retornar true.
//segundo: O telefone n�o pode estar ocupado. flag_contains(phone_buzy)==false.
//Terceiro: A pessoa n�o pode ter feito uma liga��o anteriormente. ps->flag_contains(ps_remove)==false
//Caso lck.try_lock retorne true, ningu�m mais conseguir� o acesso exclusivo at� que o bloqueio seja desfeito com unlock, ou lck seja destru�do.
if((lck.try_lock())&&(!this->flag_contains(phone_buzy))&&(!ps->flag_contains(ps_remove)))
{
_log("{} vai falar", ps->getName());
this->setflag(phone_buzy|phone_calling);
ps->setflag(ps_buzy|ps_calling);
this->person=ps;
//Fa�a a liga��o...
make_call();
this->person=shared_person();
ps->removeflag(ps_calling|ps_buzy);
ps->setflag(ps_remove);
this->removeflag(phone_buzy|phone_calling);
//break;
}
else//Uma ou mais das condi��es a cima n�o foram atendidas...
{
_log("{} est� aguardando.", ps->getName());
//Espere um tempo antes de tentar fazer a liga��o de novo...
waitms(random_int32(3000, 6000));
}
}
}

//Simula uma liga��o telef�nica...
void PublicPhone::make_call()
{
int32 x=random_int32(5000, 12000);
_log("{} est� falando no telefone...", person->getName());
waitms(x);
_log("{} desligou o telefone e saiu.", person->getName());
}
#endif
