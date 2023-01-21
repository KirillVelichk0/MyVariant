#include <iostream>
#include <type_traits>
#include <array>
#include <memory>
#include <cstring>
template<class Predicate, class T>
void CallP(Predicate pr, void* data){
    T* casted = reinterpret_cast<T*>(data);
    pr(*casted);
}
template <class T>
void Clearer(void* data){

    T* casted = reinterpret_cast<T*>(data);
    casted->~T(); //легально в шаблоне, даже если деструктор отсутствует


}
template <std::int16_t curN = 0, class T, class Val, class... Vals>
constexpr std::int16_t FindNumber(){

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
    std::int16_t curT;
    std::array<char, FindMaxSize<0, PTypes...>()> data;


public:
    MyVariant(){
        this->curT = -1;
    }
    ~MyVariant(){
        this->clear();
    }
    template<class T>
    MyVariant(T&& val){
        constexpr auto nextN = FindNumber<0, T, PTypes...>();
        this->curT = nextN;
        if constexpr(nextN != -1){
            using valueT = std::decay_t<T>;

            new(this->data.data()) valueT(std::forward<T>(val));






        }

    }
    template <class T>
    MyVariant& operator=(T&& val){
        constexpr auto nextN = FindNumber<0, T, PTypes...>();
        if constexpr(nextN != -1){
            this->clear();
            using valueT = std::decay_t<T>;
            new(this->data.data()) valueT(std::forward<T>(val));



            this->curT = nextN;

        }
        return *this;
    }
    template <class Functor>
    void Call(Functor& f){
        if(this->curT != -1){
            constexpr std::array<void(*)(std::decay_t<Functor>, void*), sizeof... (PTypes)> cont{&CallP<std::decay_t<Functor>, PTypes>...};
            cont[this->curT](f, this->data.data());
        }
    }
    void clear(){
        if(this->curT != -1){
            constexpr std::array<void(*)(void*), sizeof... (PTypes)> cont{&Clearer<PTypes>...};
            cont[this->curT](this->data.data());
        }

    }
};

int main()
{
    std::shared_ptr<int> sh = std::make_shared<int>(8);


    char a = 'a';
    std::cout << sh.use_count();
    MyVariant<int, char,double, bool, const std::shared_ptr<int>> v(sh);
    std::cout << sh.use_count();
    v = 'a';
    std::cout << sh.use_count();
    auto pr = [](auto data){
        std::cout<< std::endl << data << std::endl;
    };

    v.Call(pr);
    v = true;
    v.Call(pr);

    v= 13.3;
    v.Call(pr);
    v = sh;
    std::cout << sh.use_count();
    v = 'a';
    std::cout << sh.use_count();
}

