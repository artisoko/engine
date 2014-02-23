#include <mutex>
#include <condition_variable>

using namespace std;

namespace artisoko {
	namespace core {
		namespace util {

			class semaphore{
			private:
				mutex mtx;
				condition_variable cv;
				int count;

			public:
				semaphore(int count_) :count(count_){ ; }
				void notify()
				{
					unique_lock<mutex> lck(mtx);
					++count;
					cv.notify_one();

				}
				void wait()
				{
					unique_lock<mutex> lck(mtx);

					while (count == 0){
						cv.wait(lck);
					}

					count--;
				}
				bool trywait()
				{
					unique_lock<mutex> lock(mtx);
					if (count)
					{
						--count;
						return true;
					}
					else
					{
						return false;
					}
				}
			};
		}
	}
}