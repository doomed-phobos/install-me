#pragma once
#include <stdexcept>

namespace utils {
   template<typename T>
   class Expected {
   public:
      Expected(const T& value) : m_value(value), m_hasValue(true) {}
      Expected(T&& value) : m_value(std::move(value)), m_hasValue(true) {}
      Expected(const Expected& o) : m_hasValue(o.m_hasValue) {
         if(m_hasValue)
            new (&m_value) T(o.m_value);
         else
            new (&m_spam) std::exception_ptr(o.m_spam);
      }
      Expected(Expected&& o) : m_hasValue(o.m_hasValue) {
         if(m_hasValue)
            new (&m_value) T(std::move(o.m_value));
         else
            new (&m_spam) std::exception_ptr(std::move(o.m_spam));
      }
      template<typename Exception,
         std::enable_if_t<std::is_convertible_v<Exception, std::exception>, int> = 0>
      Expected(const Exception& e) : m_spam(std::make_exception_ptr(e)), m_hasValue(false) {}
      Expected(std::exception_ptr&& e) : m_hasValue(false) {
         new (&m_spam) std::exception_ptr(std::move(e));
      }
      ~Expected() {}

      T& get() {
         if(!isValid())
            std::rethrow_exception(m_spam);
         
         return m_value;
      }
      const T& get() const {
         if(!isValid())
            std::rethrow_exception(m_spam);

         return m_value;
      }
      
      bool isValid() const {return m_hasValue;}

      operator bool() const {return isValid();}
   private:
      Expected() {}

      union {
         T m_value;
         std::exception_ptr m_spam;
      };
      bool m_hasValue;
   };

   template<>
   class Expected<void> {
   public:
      Expected() : m_spam() {}
      Expected(const Expected& o) {
         if(!o.isValid())
            m_spam = o.m_spam;         
      }
      Expected(Expected&& o) {
         if(!o.isValid())
            m_spam = std::move(o.m_spam);
      }
      template<typename Exception,
         std::enable_if_t<std::is_convertible_v<Exception, std::exception>, int> = 0>
      Expected(const Exception& e) : m_spam(std::make_exception_ptr(e)) {}

      template<typename T,
         std::enable_if_t<!std::is_void_v<T>, int> = 0>
      Expected<T> movetoExpectedT() {
         return Expected<T>(std::move(m_spam));
      }

      void get() const {
         if(!isValid())
            std::rethrow_exception(m_spam);
      }

      bool isValid() const {
         return !m_spam;
      }

      void swap(Expected& o) {
         if(this == &o)
            return;

         std::swap(m_spam, o.m_spam);
      }

      operator bool() const {return isValid();}

      Expected& operator=(const Expected& o) {
         Expected(o).swap(*this);

         return *this;
      }
      Expected& operator=(Expected&& o) {
         Expected(std::move(o)).swap(*this);

         return *this;
      }
   private:
      std::exception_ptr m_spam;
   };
} // namespace utils