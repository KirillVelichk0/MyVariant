#include <iostream>
#include <type_traits>
#include <array>
#include <memory.h>
#include <cstring>
template<class Predicate, class T>
void CallP(Predicate pr, void* data){
    T* casted = reinterpret_cast<T*>(data);
    pr(*casted);
}
template <std::size_t curN = 0, class T, class Val, class... Vals>
constexpr std::size_t FindNumber(){

    if constexpr(std::is_same_v<std::decay_t<T>, std::decay_t<Val>>){
        return curN;
    }
    else if constexpr(sizeof...(Vals) != 0){
        return FindNumber<curN + 1, T, Vals...>();
    }
    else{
        return -1;
    }
}
template <long unsigned int CurMax = 0, class T, class... Types>
constexpr long unsigned int FindMaxSize(){
    constexpr long unsigned int sZT = sizeof(T);
    if constexpr(sZT > CurMax){
        if constexpr(sizeof...(Types) != 0){
            return FindMaxSize<sZT, Types...>();
        }
        else{
            return sZT;
        }
    }
    else{
        if constexpr(sizeof...(Types) != 0){
            return FindMaxSize<CurMax, Types...>();
        }
        else{
            return CurMax;
        }
    }
}
template<class... PTypes>
class MyVariant{
private:
    std::size_t curT;
    std::array<char, FindMaxSize<0, PTypes...>()> data;


public:
    MyVariant(){
        this->curT = -1;
    }
    template<class T>
    MyVariant(T val){
        std::memcpy(&(this->data), &val, sizeof(val));
        this->curT = FindNumber<0, T, PTypes...>(); // отбрасывание ссылочности есть внутри
    }
    template <class T>
    MyVariant& operator=(T val){
        std::memcpy(&(this->data), &val, sizeof(val));
        this->curT = FindNumber<0, T, PTypes...>(); // отбрасывание ссылочности есть внутри
        return *this;
    }
    template <class Functor>
    void Call(Functor& f){
        if(this->curT != -1){
            constexpr std::array<void(*)(std::decay_t<Functor>, void*), sizeof... (PTypes)> cont{&CallP<std::decay_t<Functor>, PTypes>...};
            cont[this->curT](f, this->data.data());
        }
    }
};

int main()
{
    MyVariant<int, char,double, bool> v('a');
    auto pr = [](auto data){
        std::cout<< std::endl << data << std::endl;
    };
    v.Call(pr);
    v = false;
    v.Call(pr);
    v= 13;
    v.Call(pr);
}

