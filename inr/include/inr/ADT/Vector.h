#ifndef INERTIA_ADT_VECTOR_H
#define INERTIA_ADT_VECTOR_H

#include <inr/ADT/Container.h>
#include <stdexcept>

namespace inr{

    /**
     * @brief Pretty straightforward vector implementation.
     */
    template<typename T>
    class vector : public resizeable_container<T>{
        size_t _size;
    public:
        vector() : inr::resizeable_container<T>(){};

        size_t size() const noexcept{
            return _size;
        }

        bool empty() const noexcept{
            return size() == 0;
        }

        const T& front() const noexcept{
            return *this->data();
        }

        T& front() noexcept{
            return *this->data();
        }

        const T& back() const noexcept{
            return *(this->data() + (size()-1));
        }

        T& back() noexcept{
            return *(this->data() + (size()-1));
        }

        T& at(size_t pos){
            if(pos >= size()){
                throw std::out_of_range("inr::vector");
            }
            return *(this->data() + pos);
        }

        const T& at(size_t pos) const{
            if(pos >= size()){
                throw std::out_of_range("inr::vector");
            }
            return *(this->data() + pos);
        }
    };

}

#endif // INERTIA_ADT_VECTOR_H
