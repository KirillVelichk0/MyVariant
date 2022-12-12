#include <iostream>
#include <type_traits>
#include <array>
#include <memory.h>
#include <cstring>
template <int curN = 0, class T, class Val, class... Vals>
constexpr int FindNumber(){

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
    int curT;
    std::array<char, FindMaxSize<0, PTypes...>()> data;
    template<int CurSt = 0, class Functor, class T, class... Types>
    void FindAndCall(Functor& f){
        if(this->curT == CurSt){
            T data;
            std::memcpy(&data, &(this->data), sizeof(T));
            f(data);
        }
        else if constexpr (sizeof...(Types) != 0){
            FindAndCall<CurSt + 1, Functor, Types...>(f);
        }
    }
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
            this->FindAndCall<0, Functor, PTypes...>(f);
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
}

