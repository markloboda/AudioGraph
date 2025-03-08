#include <vector>
#include <stdexcept>

template <typename T>
class CircularBuffer
{
private:
   std::vector<T> buffer;
   size_t head;
   size_t tail;
   size_t capacity;
   bool isFull;

public:
   explicit CircularBuffer(size_t size)
      : buffer(size), head(0), tail(0), capacity(size), isFull(false)
   {
   }

   void push(const T& value)
   {
      buffer[head] = value;
      if (isFull)
      {
         tail = (tail + 1) % capacity;
      }
      head = (head + 1) % capacity;
      isFull = head == tail;
   }

   T pop()
   {
      if (empty())
      {
         throw std::runtime_error("Buffer is empty");
      }
      T value = buffer[tail];
      isFull = false;
      tail = (tail + 1) % capacity;
      return value;
   }

   T* pop(int n)
   {
      if (n < 0 || n > size())
      {
         throw std::out_of_range("Invalid number of elements to pop");
      }
      T* values = new T[n];
      for (int i = 0; i < n; ++i)
      {
         values[i] = pop();
      }
      return values;
   }

   T peek(int index) const
   {
      if (index < 0 || index >= size())
      {
         throw std::out_of_range("Index out of range");
      }
      return buffer[(tail + index) % capacity];
   }

   void clear()
   {
      head = 0;
      tail = 0;
      isFull = false;
   }

   void resize(size_t newSize)
   {
      buffer = std::vector<T>(newSize);
      capacity = newSize;
      head = 0;
      tail = 0;
      isFull = false;
   }

   void resizeAndCopy(size_t newSize)
   {
      std::vector<T> newBuffer(newSize);
      size_t currentSize = size();

      for (size_t i = 0; i < std::min(currentSize, newSize); ++i)
      {
         newBuffer[i] = buffer[(tail + i) % capacity];
      }

      buffer = std::move(newBuffer);
      capacity = newSize;
      head = std::min(currentSize, newSize);
      tail = 0;
      isFull = (currentSize >= newSize);
   }

   T* data()
   {
      return buffer.data();
   }

   bool empty() const
   {
      return (!isFull && (head == tail));
   }

   bool full() const
   {
      return isFull;
   }

   size_t count() const
   {
      if (isFull)
      {
         return capacity;
      }
      if (head >= tail)
      {
         return head - tail;
      }
      return capacity + head - tail;
   }

   size_t size() const
   {
      return capacity;
   }
};
