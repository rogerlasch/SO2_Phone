

/**
*Escrito por Róger Matheus Lasch em Maio de 2022.
*Biblioteca de manipulação de flags threadsafe...
*Esta biblioteca foi utilizada para desenvolver o trabalho de threads de SO2.
**/
#ifndef BASICFLAGS_H
#define BASICFLAGS_H


/**
*Em vez de criar uma variável booleana para cada comportamento, eu crio constantes e uso uma operação bit a bit para armazenar tudo em um inteiro não assinado.
*Como cada variável booleana ocupa 1 único bit, e um unsigned int, na maioria das plataformas tem 32 bits, consigo armazenar até trinta e duas flags em um único inteiro.
*As flags devem seguir um padrão para que isso seja possível.
*Elas sempre devem ser o dobro da anterior como por exemplo: 1, 2, 4, 8, 16, 32, etc...
*Tomando como exemplo um inteiro de 4 bits, internamente se tudo estiver desativado, ele será isso:
*0000
*Se minha constante tiver valor 4, ele irá ativar o terceiro bit, ficando a sim:
*0100
***
*Sempre que uma operação for feita, seja de consulta, ou definição ou remoção de determinada flag, um acesso de leitura ou gravação é adquirido. Isso é feito por meio da classe shared_mutex.
**/
class BasicFlags
{
private:
//Armazena as flags...
uint32 flags;
/*Objeto que controlará o acesso a variável... O mutable significa que ele poderá ser mutável com o qualificador const em algumas situações.*/
mutable std::shared_mutex mtx_flags;
public:
//Construtor, construtor de cópia, operador de atribuição e destruidor virtual.
BasicFlags();
BasicFlags(const BasicFlags& bs);
BasicFlags& operator=(const BasicFlags& bs);
virtual ~BasicFlags();
//Substitue as flags usando um operador de atribuição...
void replace_flags(uint32 masq);
//Recupera todas flags contidas de uma vez...
uint32 get_flags()const;
//Define uma, ou mais flags...
bool setflag(uint32 masq);
//Remove uma ou mais flags...
bool removeflag(uint32 masq);
//Verifica se uma, ou várias flags estão definidas...
bool flag_contains(uint32 masq)const;
};
#endif

#ifndef BASICFLAGS_IMPLEMENTATION
#define BASICFLAGS_IMPLEMENTATION
BasicFlags::BasicFlags()
{
this->flags=0;
}

BasicFlags::BasicFlags(const BasicFlags& bs)
{
*this=bs;
}

BasicFlags& BasicFlags::operator=(const BasicFlags& bs)
{
this->replace_flags(bs.get_flags());
return *this;
}

BasicFlags::~BasicFlags()
{
}
void BasicFlags::replace_flags(uint32 masq)
{
//Adquire permissão de escrita, ninguém mais poderá acessar este método enquanto a operação atual não for concluída.
std::unique_lock<std::shared_mutex> lck(this->mtx_flags);
this->flags=masq;
}

uint32 BasicFlags::get_flags()const
{
//Adquire permissão de leitura... Um ou mais threads podem ler ao mesmo tempo.
std::shared_lock lck(this->mtx_flags);
return this->flags;
}

/**
*Define uma ou mais flags com os operadores bit a bit.
*Se uma flag já estiver definida, a operação toda retornará false.
**/
bool BasicFlags::setflag(uint32 masq)
{
if(!this->flag_contains(masq))
{
std::unique_lock<std::shared_mutex> lck(this->mtx_flags);
this->flags|=masq;
return true;
}
return false;
}

/**
*Remove uma ou mais flags. Caso a flag não exista, não faça nada e retorne false.
**/
bool BasicFlags::removeflag(uint32 masq)
{
if(this->flag_contains(masq))
{
std::unique_lock<std::shared_mutex> lck(this->mtx_flags);
this->flags^=masq;
return true;
}
return false;
}

/**
*Verifica se uma ou mais flags está definida...
*Retorna true se todas as flags existirem, false em falha.
**/
bool BasicFlags::flag_contains(uint32 masq)const
{
std::shared_lock lck(this->mtx_flags);
return (this->flags&masq)==masq;
}
#endif
