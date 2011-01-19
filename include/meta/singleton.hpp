/**
 * @file
 * @date 22 juin 2010
 * @todo comment
 */

#ifndef SINGLETON_HPP_
#define SINGLETON_HPP_

/**
 * @brief Taken from boost/pool/details/singleton.hpp
 * @see boost/pool/details/singleton.hpp
 */
// T must be: no-throw default constructible and no-throw destructible
template< typename T >
struct singleton {
  private:
    struct object_creator {
        // This constructor does nothing more than ensure that instance()
        //  is called before main() begins, thus creating the static
        //  T object before multithreading race issues can come up.
        object_creator() {
          singleton< T >::instance();
        }
        inline void do_nothing() const {
        }
    };
    static object_creator create_object;

  protected:
    singleton() {};

  public:
    typedef T object_type;

    // If, at any point (in user code), singleton<T>::instance()
    //  is called, then the following function is instantiated.
    static object_type & instance() {
      // This is the object that we return a reference to.
      // It is guaranteed to be created before main() begins because of
      //  the next line.
      static object_type obj;

      // The following line does nothing else than force the instantiation
      //  of singleton<T>::create_object, whose constructor is
      //  called before main() begins.
      create_object.do_nothing();

      return obj;
    }
};
template< typename T >
typename singleton< T >::object_creator singleton< T >::create_object;

#endif /* SINGLETON_HPP_ */
