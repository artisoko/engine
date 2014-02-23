#ifndef _CORE_UTIL_VARCONTAINER__
#define _CORE_UTIL_VARCONTAINER__
#include<boost/mpl/list.hpp>
#include <boost/mpl/front.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/begin_end.hpp>

namespace artisoko {
	namespace core {
		namespace util {
			namespace {
				template<class T> class empty {};
			}
			//	type_l - boost::mpl::list of types of which instances will be contained
			//	additional_data - additional members connected with each instance; templated on instance type
			template<class type_l, template<class pipe_data> class additional_data = empty>
			struct varcontainer {

			private:

				template<class type_l_iter> struct instances_s {
					typedef typename boost::mpl::deref<type_l_iter>::type element_t;
					typedef instances_s<typename boost::mpl::next<type_l_iter>::type> next_t;

					additional_data<element_t> additional;
					element_t inst;
					next_t next;

				};
				template<>struct instances_s<typename boost::mpl::end<type_l>::type >{};

				instances_s< typename boost::mpl::begin<type_l>::type > instances;

				template<class opt_t>
				struct foreach_s {

					template<class instances_iter>
					static void exec_foreach(const opt_t& op, typename instances_s<instances_iter>& instance) {
						//std::cout << typeid(instance).name() << std::endl;
						op(instance);
						exec_foreach<boost::mpl::next<instances_iter>::type>(op, instance.next);
					}
					template<> static void exec_foreach<typename boost::mpl::end<type_l>::type>(const opt_t& opt, typename instances_s<typename boost::mpl::end<type_l>::type>& instance) {}
				
					template<class instances_iter>
					static bool exec_foreach_until(const opt_t& op, typename instances_s<instances_iter>& instance) {
						if (op(instance)){
							return true ; 
						}
						exec_foreach_until<boost::mpl::next<instances_iter>::type>(op, instance.next);
					}
					template<> static bool exec_foreach_until<typename boost::mpl::end<type_l>::type>(const opt_t& opt, typename instances_s<typename boost::mpl::end<type_l>::type>& instance) { return false; }
				};

			public:

				// execute functor on each var member
				template<class op_t>
				void foreach(const op_t& op){
					foreach_s<op_t>::exec_foreach(op, instances);
				}

				// execute predicate on each var member until it returns false
				template<class predicate_t>
				bool foreach_until_returns_true(const predicate_t& pred) {
					foreach_s<predicate_t>::exec_foreach_until(pred, instances);
				}
			};

		}
	}
}
#endif