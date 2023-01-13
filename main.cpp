#include <iostream>
#include <type_traits>
#include <array>
#include <memory.h>
#include <cstring>

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
    template <std::size_t curSt, std::size_t minSt, std::size_t maxSt, class Functor>
    void BinFindAndCall(Functor& f){
        if constexpr( curSt > 0 && curSt < sizeof...(PTypes)){
            if(this->curT == curSt){
                using T = std::tuple_element_t<curSt, std::tuple<PTypes...>>;
                T data;
                std::memcpy(&data, &(this->data), sizeof(T));
                f(data);
            }
            else {
                if(curSt < this->curT)
                {
                    constexpr std::size_t nextSt = std::max((curSt + maxSt)/2, curSt + 1);
                    this->BinFindAndCall<nextSt, curSt + 1, maxSt, Functor>(f);
                }
                else {
                    constexpr std::size_t nextSt = std::min((minSt + curSt) / 2, curSt - 1);
                    this->BinFindAndCall<nextSt, minSt, curSt - 1, Functor>(f);
                }

            }
        }
    }
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
            constexpr std::size_t startSt = 0 + sizeof...(PTypes) - 1;

            this->BinFindAndCall<startSt, 0, sizeof...(PTypes) - 1, Functor>(f);

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

