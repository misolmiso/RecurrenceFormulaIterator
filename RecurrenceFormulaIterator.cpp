#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <iterator>

//
//
//コピーしてtupleをfunctionにしただけ
//http://d.hatena.ne.jp/faith_and_brave/20090402/1238659059
//http://d.hatena.ne.jp/faith_and_brave/20090415/1239785910
//
//

template <class T, class Seq>
struct cons;

template <class T, class... Seq>
struct cons<T, std::function<T(Seq...)> > {
    typedef std::function<T(T, Seq...)> type;
};

template <size_t N, class T>
struct replicate {
    typedef typename cons<T, typename replicate<N-1, T>::type>::type type;
};

template <class T>
struct replicate<0, T> {
    typedef std::function<T()> type;
};

//////////////////////////////////////////

template <size_t N, typename T>
class RecurrenceFormula;

template <size_t N, typename T, class F>
class RecurrenceFormulaIterator;

template <size_t N, typename T>
class RecurrenceFormula {
    typedef typename replicate<N, T>::type F;

    const F _f;

public:
    RecurrenceFormula(const F& f) : _f(f) {}
    
    template<typename ...A>
    T operator()(const A&... a) const {
        return _f(a...);
    }

    template <typename ...A>
    RecurrenceFormulaIterator<N, T, F> begin(const A&... a) {
        return RecurrenceFormulaIterator<N, T, F>(_f, a...);
    }

    RecurrenceFormulaIterator<N, T, F> end(const size_t c) {
        return RecurrenceFormulaIterator<N, T, F>(c, _f);
    }
};


template <size_t N, typename T, class F>
class RecurrenceFormulaIterator 
    : public RecurrenceFormulaIterator<N-1, T, F>
{
    friend class RecurrenceFormula<N, T>;
    friend class RecurrenceFormulaIterator<N + 1, T, F>;

    typedef RecurrenceFormulaIterator<N-1, T, F> RE;
    T _v;


private:
    template<typename ...R>
    RecurrenceFormulaIterator(const F& f, const T& v, const R&... r) :
        _v(v), RE::RecurrenceFormulaIterator(f, r...) {}

    template<typename ...R>
    RecurrenceFormulaIterator(const size_t count, const F& f, const T& v, const R&... r) :
        _v(v), RE::RecurrenceFormulaIterator(count, f, r...) {}

    RecurrenceFormulaIterator(const size_t count, const F& f) :
        RE::RecurrenceFormulaIterator(count, f) {}


    template <typename ...A>
    T advance(A... a) {
        T temp = _v;
        _v = RE::advance(_v, a...);
        return temp;
    }


public:
    virtual ~RecurrenceFormulaIterator() {}


    T operator*() const {
        return _v;
    }

    RecurrenceFormulaIterator& operator++() {
        advance();
        return *this;
    }

    RecurrenceFormulaIterator& operator++(int) {
        advance();
        return *this;
    }

    bool operator==(const RecurrenceFormulaIterator<N, T, F>& t) const {
        return RE::operator==(t);
    }

    bool operator!=(const RecurrenceFormulaIterator<N, T, F>& t) const {
        return !(*this == t);
    }
    

    T* operator->() {
        return &_v;
    }
};

template <typename T, class F>
class RecurrenceFormulaIterator<0, T, F>
    : public std::iterator<std::input_iterator_tag, T>
{
    friend class RecurrenceFormula<0, T>;
    friend class RecurrenceFormulaIterator<1, T, F>;

    size_t _count;
    const F& _f;

private:
    explicit RecurrenceFormulaIterator(const F& f) : _count(0), _f(f) {}
    RecurrenceFormulaIterator(const size_t count, const F& f) : _count(count), _f(f) {}

    template <typename ...A>
    T advance(A... a) {
        ++_count;
        return _f(a...);
    }
    
public:
    virtual ~RecurrenceFormulaIterator() {}


    T operator*() const {
        return _f();
    }

    RecurrenceFormulaIterator& operator++() const {
        return *this;
    }

    RecurrenceFormulaIterator& operator++(int) const {
        return *this;
    }

    bool operator==(const RecurrenceFormulaIterator<0, T, F>& t) const {
        return (_count == t._count) && (&_f == &(t._f));
    }

    bool operator!=(const RecurrenceFormulaIterator<0, T, F>& t) const {
        return !(*this == t);
    }

    const F* operator->() const {
        return &_f;
    }
};


int main() {
    RecurrenceFormula<2, int> f(
        [](int a1, int a2) {return a1 + a2;});

    auto b = f.begin(0, 1);
    auto e = f.end(10);


    std::copy(b, e, std::ostream_iterator<int>(std::cout, " "));
    std::cout << std::endl;
    std::cout << std::accumulate(b, e, 0) << std::endl;

    return 0;
}


